#ifdef WTF
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RegenVisitor : public RecursiveASTVisitor<RegenVisitor>
{
public:

    explicit RegenVisitor(ASTContext *Context) : Context(Context) {}

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
    {
        FullSourceLoc loc = Context->getFullLoc(Declaration->getLocStart());
        llvm::outs() << loc.getFileEntry()->getName() << ":"
                     << loc.getSpellingLineNumber() << ":"
                     << loc.getSpellingColumnNumber() << ": "
                     << Declaration->getQualifiedNameAsString() << "\n";
        return false;
    }

private:

    ASTContext *Context;
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCode(new RegenAction, argv[1]);
  }
}
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/CodeGen/ObjectFilePCHContainerOperations.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"

using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp(
    "\tFor example, to run regen-tool on all files in a subtree of the\n"
    "\tsource tree, use:\n"
    "\n"
    "\t  find path/in/subtree -name '*.cpp'|xargs regen-tool\n"
    "\n"
    "\tor using a specific build path:\n"
    "\n"
    "\t  find path/in/subtree -name '*.cpp'|xargs regen-tool -p build/path\n"
    "\n"
    "\tNote, that path/in/subtree and current directory should follow the\n"
    "\trules described above.\n"
    "\n"
);

#define _c4desc(opt_) cl::desc(Options->getOptionHelpText(options::opt_)), cl::cat(RegenToolCategory)
#define _c4expl(desc_) cl::desc(desc_), cl::cat(RegenToolCategory)

static cl::OptionCategory RegenToolCategory("regen-tool options");
static std::unique_ptr<opt::OptTable> Options(createDriverOptTable());
static cl::opt<std::string> ConfigFile("config-file", _c4expl("path to the configuration file"));
static cl::opt<bool       > ASTDump("ast-dump", _c4desc(OPT_ast_dump));
static cl::opt<bool       > ASTList("ast-list", _c4desc(OPT_ast_list));
static cl::opt<bool       > ASTPrint("ast-print", _c4desc(OPT_ast_print));
static cl::opt<std::string> ASTDumpFilter("ast-dump-filter", _c4desc(OPT_ast_dump_filter));

#undef _c4desc
#undef _c4expl


namespace {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RegenVisitor : public clang::RecursiveASTVisitor<RegenVisitor>
{
public:

    explicit RegenVisitor(clang::ASTContext *Context) : Context(Context) {}

    bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration)
    {
        clang::FullSourceLoc loc = Context->getFullLoc(Declaration->getLocStart());
        llvm::outs() << loc.getFileEntry()->getName() << ":"
                     << loc.getSpellingLineNumber() << ":"
                     << loc.getSpellingColumnNumber() << ": "
                     << Declaration->getQualifiedNameAsString() << "\n";
        return false;
    }

private:

    clang::ASTContext *Context;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RegenConsumer : public clang::ASTConsumer
{
public:

    explicit RegenConsumer(clang::ASTContext *Context) : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context)
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:

    RegenVisitor Visitor;

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RegenAction : public clang::ASTFrontendAction
{
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
  {
    return std::unique_ptr<clang::ASTConsumer>(new RegenConsumer(&Compiler.getASTContext()));
  }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RegenToolActionFactory
{
public:

    std::unique_ptr<clang::ASTConsumer> newASTConsumer()
    {
        if     (ASTList)  return clang::CreateASTDeclNodeLister();
        else if(ASTDump)  return clang::CreateASTDumper(ASTDumpFilter, /*DumpDecls=*/true, /*Deserialize=*/false, /*DumpLookups=*/false);
        else if(ASTPrint) return clang::CreateASTPrinter(nullptr, ASTDumpFilter);
        //return llvm::make_unique<RegenAction>();
        return llvm::make_unique<clang::ASTConsumer>();
    }

};

} // namespace


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, const char **argv)
{
    llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

    // Initialize targets for clang module support.
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    CommonOptionsParser opts(argc, argv, RegenToolCategory);
    ClangTool tool(opts.getCompilations(), opts.getSourcePathList());

    // Clear adjusters because -fsyntax-only is inserted by the default chain.
    tool.clearArgumentsAdjusters();
    tool.appendArgumentsAdjuster(getClangStripOutputAdjuster());

    RegenToolActionFactory rfac;
    std::unique_ptr<FrontendActionFactory> ffac = newFrontendActionFactory(&rfac);

    return tool.run(FrontendFactory.get());

    clang::tooling::runToolOnCode(new RegenAction, argv[1]);
}
