
local function get_src(dir, recursive)
	return Glob {
		Dir = dir,
		Extensions = { ".cpp", ".c", ".h", ".s" },
		Recursive = recursive and true or false,
	}
end

-----------------------------------------------------------------------------------------------------------------------

local function get_rs_src(dir)
	return Glob {
		Dir = dir,
		Extensions = { ".rs" },
		Recursive = true,
	}
end

------------------------ 68k emulation stuff ------------------------

DefRule {
	Name = "m68kmake",
	Pass = "CodeGeneration",
	Command = "$(M68KMAKE) $(<) $(M68KEMUPATH)/m68k_in.c",
	ImplicitInputs = { "$(M68KMAKE)", "$(M68KEMUPATH)/m68k_in.c" },

	Blueprint = {
		TargetDir = { Required = true, Type = "string", Help = "Input filename", },
	},

	Setup = function (env, data)
		return {
			InputFiles = { data.TargetDir },
			OutputFiles = { 
				"$(OBJECTDIR)/_generated/m68kopac.c", 
				"$(OBJECTDIR)/_generated/m68kopdm.c", 
				"$(OBJECTDIR)/_generated/m68kopnz.c", 
				"$(OBJECTDIR)/_generated/m68kops.c", 
				"$(OBJECTDIR)/_generated/m68kops.h", 
			},
		}
	end,
}

-----------------------------------------------------------------------------------------------------------------------

StaticLibrary {
	Name = "68k_emu_core",
	Env = {
        { CCOPTS = { "-Wno-unused-parameter"  } ; Config = "mac*-*-*" },

		CPPPATH = { 
			"native/musashi",
			"$(OBJECTDIR)/_generated",
		},
	},
	
	Sources = {
		"native/musashi/m68kcpu.c",
		"native/musashi/m68kdasm.c",

		m68kmake {
			TargetDir = "$(OBJECTDIR)/_generated",
		},
	},
}

-----------------------------------------------------------------------------------------------------------------------

Program {
	Name = "m68kmake",
	Pass = "BuildTools",
	Target = "$(M68KMAKE)",
	Sources = { "$(M68KEMUPATH)/m68kmake.c" },
}

-----------------------------------------------------------------------------------------------------------------------

RustProgram {
	Name = "open_depacker",
	CargoConfig = "main/Cargo.toml",
	Sources = { 
		get_rs_src("main"),
		get_rs_src("musashi"),
	},

	Depends = { "68k_emu_core" },
}

-----------------------------------------------------------------------------------------------------------------------

Default "m68kmake"
Default "open_depacker"


