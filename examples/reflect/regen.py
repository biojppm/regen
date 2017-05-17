
import c4.regen as regen


egen = regen.EnumGenerator(
    hdr_preamble='#include "enum.hpp"',
    hdr="""\
template<> const EnumSymbols< {{enum.type}} > esyms();
{% if enum.class_offset > 0 %}
template<> inline size_t eoffs_cls< {{enum.type}} >()
{
    // same as strlen("{{enum.class_str}}")
    return {{enum.class_offset}};
}
{% endif %}
{% if enum.prefix_offset > enum.class_offset %}
template<> inline size_t eoffs_pfx< {{enum.type}} >()
{
    // same as strlen("{{enum.class_str}}{{enum.prefix}}")
    return {{enum.prefix_offset}};
}
{% endif %}
""",
    src="""\
template<> const EnumSymbols< {{enum.type}} > esyms()
{
    static const EnumSymbols< {{enum.type}} >::Sym vals[] = {
        {% for e in enum.symbols %}
        { {{e.name}}, "{{e.name}}"},
        {% endfor %}
    };
    EnumSymbols< {{enum.type}} > r(vals);
    return r;
}
"""
)

# ------------------------------------------------------------------------------

serialize = regen.ClassGenerator(
    name="serialize",
    hdr_preamble='#include "serialize.hpp"',
    hdr="""\
namespace c4 {
template <{{tpl_params}}>
struct serialize_category< {{type}} >
{
    enum : int { value = (int)SerializeCategory_e::METHOD };
};
} // end namespace c4
{%if tpl_params %}
template <{{tpl_params}}>
{% endif %}
template <class Stream>
void {{type}}::serialize(c4::Archive< Stream > &a, const char* name)
{
    {% for m in members %}
    serialize< {{m.type}} >(a, "{{m.name}}", &{{m.name}});
    {% endfor %}
}
""",
    src="""\
""",
)

# ------------------------------------------------------------------------------

imgui = regen.ClassGenerator(
    name="imgui",
    hdr="""\
// {{type}}
""",
    src="""\
// {{type}}
""",
)

# ------------------------------------------------------------------------------

writer = regen.ChunkWriterGenFile()

# -----------------------------------------------------------------------------
if __name__ == "__main__":
    regen.run(writer, egen, [serialize, imgui])
