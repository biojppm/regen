
import c4.regen as regen


egen = regen.EnumGenerator(
    hdr="""
template<> const EnumSymbols< {{enum.type}} > esyms();
""",
    inl="""
{% if enum.superclass_len > 0 %}
template<> inline constexpr uint16_t eoffs< {{enum.type}} >()
{
    return {{enum.superclass_len}}; // same as strlen("{{enum.superclass}}")
}
{% endif %}
""",
    src="""
template<> const EnumPairs< {{enum.type}} > epairs()
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

# ------------------------------------------------------------------------------

serialize = regen.ClassGenerator(
    name="serialize",
    hdr="""
template<{{tpl_params}}>
void serialize< {{type}} >(Archive &a, const char *name, {{type}} *var);
""",
    src="""
template<{{tpl_params}}>
void serialize< {{type}} >(Archive &a, const char *name, {{type}} *var)
{
    {% for m in members %}
    serialize< {{m.type}} >(a, "{{m.name}}", &var->{{m.name}});
    {% endfor %}
}
""",
)

# ------------------------------------------------------------------------------

writer = regen.ChunkWriterGenFile()

# -----------------------------------------------------------------------------
if __name__ == "__main__":
    regen.run(writer, egen, [serialize])
