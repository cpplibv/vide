# To enabled project root .gdbinit on Windows
# Add the following line to C:/Users/<user>/.config/gdb/gdbinit:
#		set auto-load safe-path /

print ("Loading Vide project .gdbinit")



# === Break points for shared std library ==========================================================

set breakpoint pending on
break abort
break throw



# === Pretty printers ==============================================================================

# source path/to/type.gdbinit.py
