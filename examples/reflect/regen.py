
import c4.regen as regen


egen = regen.EnumGenerator(
    hdr="""
// {{enum.type}}
""",
    inl="""
""",
    src="""
"""
)

# ------------------------------------------------------------------------------

serialize = regen.ClassGenerator(
    name="serialize",
    hdr="""
// {{type}}
""",
    src="""
// {{type}}
""",
)

# ------------------------------------------------------------------------------

imgui = regen.ClassGenerator(
    name="imgui",
    hdr="""
// {{type}}
""",
    src="""
// {{type}}
""",
)

# ------------------------------------------------------------------------------

writer = regen.ChunkWriterGenFile()

# -----------------------------------------------------------------------------
if __name__ == "__main__":
    regen.run(writer, egen, [serialize, imgui])
