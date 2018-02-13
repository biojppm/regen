#ifndef _C4_REGEN_HPP_
#define _C4_REGEN_HPP_

#include <vector>
#include <memory>
#include <cstdio>

#include <c4/yml/yml.hpp>
#include <c4/tpl/engine.hpp>

namespace c4 {
namespace regen {

using cspan    = c4::yml::cspan;
using  span    = c4::yml::span;
using DataNode = c4::yml::NodeRef;
using DataTree = c4::yml::Tree;

using Engine   = c4::tpl::Engine;
using Rope     = c4::tpl::Rope;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void file_get_contents(const char *filename, std::vector< char > *v)
{
    std::FILE *fp = std::fopen(filename, "rb");
    if(fp)
    {
        std::fseek(fp, 0, SEEK_END);
        v->resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&(*v)[0], 1, v->size(), fp);
        std::fclose(fp);
    }
    throw(errno);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Region
{
    struct Pos { size_t offset, line, col };

    const char * m_file;
    Pos          m_start;
    Pos          m_end;
};

struct CodeEntity : public Region
{
};

struct Annotation : public CodeEntity
{
    cspan m_key;
    cspan m_val;
};

struct Tag : public CodeEntity
{
    std::vector< Annotation > m_annotations;
};

struct Var : public CodeEntity
{
    cspan m_name;
    cspan m_type_name;
};

struct Class;
struct Member : public Var
{
    Class *m_class;

};

struct Enum;
struct EnumSymbol : public CodeEntity
{
    Enum *m_enum;

    cspan m_sym;
    cspan m_val;
};

/** an entity which will originate code; ie, cause code to be generated */
struct Originator : public CodeEntity
{

};

struct Class : public Originator
{
    std::vector< Member > m_members;
};

struct Enum : public Originator
{
    std::vector< EnumSymbol > m_symbols;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct CodeChunk
{
    Generator *m_generator;
    CodeEntity m_originator;
    Rope m_hdr_preamble;
    Rope m_inl_preamble;
    Rope m_src_preamble;
    Rope m_hdr;
    Rope m_inl;
    Rope m_src;
};

struct Generator
{
    cspan  m_name;
    cspan  m_tag;

    Engine m_hdr_preamble;
    Engine m_inl_preamble;
    Engine m_src_preamble;
    Engine m_hdr;
    Engine m_inl;
    Engine m_src;

    bool m_empty;

    Generator() :
        m_name(),
        m_tag(),
        m_hdr_preamble(),
        m_inl_preamble(),
        m_src_preamble(),
        m_hdr(),
        m_inl(),
        m_src(),
        m_empty(true)
    {
    }

    void load(DataNode const& n)
    {
        m_name = n.key();
        n["tag"] >> m_tag;
        load_tpl(n, "hdr_preamble", &m_hdr_preamble);
        load_tpl(n, "inl_preamble", &m_inl_preamble);
        load_tpl(n, "src_preamble", &m_src_preamble);
        load_tpl(n, "hdr", &m_hdr);
        load_tpl(n, "inl", &m_inl);
        load_tpl(n, "src", &m_src);
    }

    void load_tpl(DataNode const& n, cspan const& name, Engine *eng)
    {
        cspan src;
        n.get_if(name, &src);
        if(src.empty()) return;
        eng->parse(src);
        m_empty = false;
    }

    void generate(Originator const& o, DataNode *root, CodeChunk *ch)
    {
        if(m_empty) return;
        ch->m_generator = &this;
        ch->m_originator = o;
        create_prop_tree(root);
        render(*root, ch);
    }

    virtual void create_prop_tree(Originator const& o, DataNode *root) = 0;

    void render(DataNode const& properties, CodeChunk *ch)
    {
        render_tpl(properties, m_hdr_preamble, ch->m_hdr_preamble);
        render_tpl(properties, m_inl_preamble, ch->m_inl_preamble);
        render_tpl(properties, m_src_preamble, ch->m_src_preamble);
        render_tpl(properties, m_hdr, ch->m_hdr);
        render_tpl(properties, m_inl, ch->m_inl);
        render_tpl(properties, m_src, ch->m_src);
    }

    void render_tpl(DataNode const& n, Engine const& eng, Rope *rp)
    {
        *rp = eng.rope();
        eng.render(n, rp);
    }
};

struct EnumGenerator : public Generator
{
    void create_prop_tree(Originator const& o, DataNode *root) override final
    {

    }
};

struct ClassGenerator : public Generator
{
    void create_prop_tree(Originator const& o, DataNode *root) override final
    {

    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct SourceFile : public CodeEntity
{
    struct OriginatorPos
    {
        bool is_class;
        size_t pos;
    };

    std::vector< OriginatorPos > m_pos;
    std::vector< Enum > m_enums;
    std::vector< Class > m_classes;

    std::vector< CodeChunk > m_chunks;

    void generate_class_code(ClassGenerator const& g)
    {
        DataTree props;
        DataNode root = props.rootref();

        m_chunks.resize(m_pos.size());
        size_t count = 0;
        for(auto const& p : m_pos)
        {
            if(p.is_class)
            {
                g.generate(m_classes[p.pos], &root, &m_chunks[count]);
            }
            ++count;
        }
    }

    void generate_enum_code(EnumGenerator const& g)
    {
        DataTree props;
        DataNode root = props.rootref();

        m_chunks.resize(m_pos.size());
        size_t count = 0;
        for(auto const& p : m_pos)
        {
            if( ! p.is_class)
            {
                g.generate(m_enum[p.pos], &root, &m_chunks[count]);
            }
            ++count;
        }
    }

public:

    struct const_iterator
    {
        const_iterator(SourceFile const* s, size_t pos) : s(s), pos(pos) {}
        SourceFile *s;
        size_t pos;

        using value_type = Originator const;

        value_type& operator*  () const { C4_ASSERT(pos >= 0 && pos < m_pos.size()); auto &p = m_pos[pos]; return p.is_class ?  m_classes[p.pos] :  m_enums[p.pos]; }
        value_type* operator-> () const { C4_ASSERT(pos >= 0 && pos < m_pos.size()); auto &p = m_pos[pos]; return p.is_class ? &m_classes[p.pos] : &m_enums[p.pos]; }
    };

    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end  () const { return const_iterator(this, m_pos.size()); }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


struct Writer
{
    virtual void write(SourceFile const& src) = 0;

    static Writer* create(cspan type);
};


struct WriterStdOut : public Writer
{
    void write(SourceFile const& src) override final
    {
    }
};

struct WriterGenFile : public Writer
{
    void write(SourceFile const& src) override final
    {
    }
};

struct WriterGenGroup : public Writer
{
    void write(SourceFile const& src) override final
    {
    }
};

struct WriterSameFile : public Writer
{
    void write(SourceFile const& src) override final
    {
    }
};


Writer* Writer::create(cspan type);
{
    if(type == "genfile")
    {
        return new WriterGenFile();
    }
    else if(type == "gengroup")
    {
        return new WriterGenGroup();
    }
    else if(type == "samefile")
    {
        return new WriterSameFile();
    }
    else if(type == "stdout")
    {
        return new WriterStdOut();
    }
    else
    {
        C4_ERROR("unknown writer type");
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Regen
{
    std::string m_config_file_name;
    std::vector< char > m_config_file_yml;
    DataTree m_config_data;

    std::vector<  EnumGenerator > m_enum_gens;
    std::vector< ClassGenerator > m_class_gens;

    std::vector< SourceFile > m_files;
    std::unique_ptr< Writer > m_writer;

    void load_config_file(std::string const& file_name)
    {
        m_config_file_name = file_name;
        cspan cfn(m_config_file_name.begin(), m_config_file_name.end());
        file_get_contents(m_config_file_name.c_str(), m_config_file_yml);
        parse(cfn, m_config_file_yml, &m_config_data);

        DataNode n = m_config_data.rootref();

        cspan writer_type;
        m_config_data.get_if("writer", &writer_type, "genfile");
        m_writer.reset(Writer::create(writer_type));

        n = m_config_data.find_child("enum");
        if(n.valid())
        {
            for(auto const& ch : n.children())
            {
                m_enum_gens.emplace_back();
                auto &g = m_enum_gens.back();
                g.load(ch):
            }
        }

        n = m_config_data.find_child("class");
        if(n.valid())
        {
            for(auto const& ch : n.children())
            {
                m_class_gens.emplace_back();
                auto &g = m_enum_class.back();
                g.load(ch):
            }
        }
    }

    bool empty()
    {
        return m_enum_gens.size() == 0 && m_class_gens.size() == 0;
    }

};

} // namespace regen
} // namespace c4


#endif /* _C4_REGEN_HPP_ */
