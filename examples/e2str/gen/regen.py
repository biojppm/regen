#!/usr/bin/env python3

import c4.regen as regen

egen = regen.EnumGenerator(
    hdr="""
template<> const EnumSymbols< {{enum.type}} > esyms();
""",
    src="""
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
writer = regen.ChunkWriterGenFile()

#------------------------------------------------------------------------------
if __name__ == "__main__":
    regen.run(writer, egen, [])
