#!/usr/bin/env python3

import c4.regen as regen

egen = regen.EnumGenerator(
    hdr="""
template<> const EnumPairs< {{enum.type}} > enum_pairs();
""",
    src="""
template<> const EnumPairs< {{enum.type}} > enum_pairs()
{
    static const EnumAndName< {{enum.type}} > vals[] = {
        {% for e in enum.symbols %}
        { {{e.name}}, "{{e.name}}"},
        {% endfor %}
    };
    EnumPairs< {{enum.type}} > r(vals);
    return r;
}
"""
)
writer = regen.ChunkWriterGenFile()

#------------------------------------------------------------------------------
if __name__ == "__main__":
    regen.run(writer, egen, [])
