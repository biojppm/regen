#!/usr/bin/env python3

import c4.regen as regen

egen = regen.EnumGenerator(
    inl="""
template<> inline const EnumSymbols< {{enum.type}} > esyms()
{
    static const EnumSymbols< {{enum.type}} >::Sym vals[] = {
        {% for e in enum.symbols %}
        { {{e.name}}, "{{e.name}}"},
        {% endfor %}
    };
    EnumSymbols< {{enum.type}} > r(vals);
    return r;
}
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
""")

writer = regen.ChunkWriterSameFile()

# -----------------------------------------------------------------------------
if __name__ == "__main__":
    regen.run(writer, egen, [])
