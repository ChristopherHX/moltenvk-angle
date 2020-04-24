import bpy
import sys


argv = sys.argv
argv = argv[argv.index("--") + 1:] # get all args after "--"

obj_out = argv[0]

bpy.ops.wm.collada_export(filepath=obj_out, apply_modifiers=True,use_texture_copies=True)