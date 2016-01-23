require "tundra.syntax.glob"
require "tundra.syntax.rust-cargo"

-----------------------------------------------------------------------------------------------------------------------

local common = {
	Env = {
		M68KMAKE = "$(OBJECTDIR)$(SEP)m68kmake$(PROGSUFFIX)",
		M68KEMUPATH = "native/musashi",
	},
}

-----------------------------------------------------------------------------------------------------------------------

local clang_opts = {
	{ "-O0", "-g"; Config = "*-*-debug" },
	{ "-O3", "-g"; Config = "*-*-release" },
}

-----------------------------------------------------------------------------------------------------------------------
        
local gcc_opts = {
	"-I.",
	"-Wno-array-bounds", "-Wno-attributes", "-Wno-unused-value",
	"-fPIC",
	{ "-O0", "-g"; Config = "*-*-debug" },
	{ "-O3", "-g"; Config = "*-*-release" },
}
-----------------------------------------------------------------------------------------------------------------------

local win64_opts = {
	"/EHsc", "/FS", "/MT", "/W3", "/I.", "/WX", "/DUNICODE", "/D_UNICODE", "/DWIN32", "/D_CRT_SECURE_NO_WARNINGS", "/wd4200", "/wd4152", "/wd4996", "/wd4389", "/wd4201", "/wd4152", "/wd4996", "/wd4389",
	{ "/Od"; Config = "*-*-debug" },
	{ "/O2"; Config = "*-*-release" },
}

-----------------------------------------------------------------------------------------------------------------------

local macosx = {
    Env = {
        CCOPTS = { clang_opts },
        CXXOPTS = { clang_opts },
		M68KMAKE = "$(OBJECTDIR)$(SEP)m68kmake$(PROGSUFFIX)",
		M68KEMUPATH = "native/musashi",
    },
}

-----------------------------------------------------------------------------------------------------------------------

local nix = {
    Env = {
        CCOPTS = { gcc_opts },
        CXXOPTS = { gcc_opts },
		M68KMAKE = "$(OBJECTDIR)$(SEP)m68kmake$(PROGSUFFIX)",
		M68KEMUPATH = "native/musashi",
    },
}

-----------------------------------------------------------------------------------------------------------------------

local win64 = {
    Env = {
        GENERATE_PDB = "1",
        CCOPTS = { win64_opts },
        CXXOPTS = { win64_opts },
		M68KMAKE = "$(OBJECTDIR)$(SEP)m68kmake$(PROGSUFFIX)",
		M68KEMUPATH = "native/musashi",
    },
}

-----------------------------------------------------------------------------------------------------------------------

Build {
    Passes = {
        BuildTools = { Name="Build Tools", BuildOrder = 1 },
        CodeGeneration = { Name="CodeGeneration", BuildOrder = 2 },
    },

    Units = { 
    	"units.lua",
	},

    Configs = {
        Config { Name = "macosx-clang", DefaultOnHost = "macosx", Inherit = macosx, Tools = { "clang-osx", "rust" } },
        Config { Name = "win64-msvc", DefaultOnHost = { "windows" }, Inherit = win64, Tools = { "msvc", "rust" } },
        Config { Name = "nix-gcc", DefaultOnHost = { "linux" }, Inherit = nix, Tools = { "gcc", "rust" } },
    },
}


