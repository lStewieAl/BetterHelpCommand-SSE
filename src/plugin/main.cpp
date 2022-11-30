typedef long long UInt64;

inline const std::uintptr_t ScriptHelpFunctionAddr = REL::ID(22839).address();
inline const std::uintptr_t ExtractArgsHookAddr = ScriptHelpFunctionAddr + 0x6F;
extern "C" const std::uintptr_t SkipAddr = ScriptHelpFunctionAddr + 0x943;
extern "C" const std::uintptr_t ContinueAddr = ScriptHelpFunctionAddr + 0x823;
inline const std::uintptr_t TypeIDHookAddr = ScriptHelpFunctionAddr + 0x81D;
inline const std::uintptr_t ViewerStringsAddr = ScriptHelpFunctionAddr + 0x665;
inline const std::uintptr_t HelpUsageAddr = ScriptHelpFunctionAddr + 0xA43;
inline const std::uintptr_t ParamInfoToReplaceAddr = REL::ID(365802).address();
inline const std::uintptr_t TwoOptionalStringsAddr = REL::ID(368114).address();
inline const std::uintptr_t FormTypeStringsAddr = REL::ID(359121).address();
extern "C" unsigned char filterTypeID = 0;
extern "C" void HookCheckTypeID();

void __cdecl OnHelpExtractArgs(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, std::uint32_t& a_opcodeOffsetPtr, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj, RE::ScriptLocals* a_locals, const char* toFind, char* typeStr)
{
	filterTypeID = 0;

	*typeStr = '0';
	typeStr[1] = '\0';
	RE::Script::ParseParameters(a_paramInfo, a_scriptData, a_opcodeOffsetPtr, a_thisObj, a_containingObj, a_scriptObj, a_locals, toFind, typeStr);

	DWORD filterType;
	filterType = strtoul(typeStr, nullptr, 10);
	if (!filterType)
	{
		auto formTypes = (const char**)FormTypeStringsAddr;
		// search the form types enum for the identifier
		for (int i = 0; i <= 136; ++i)
		{
			if (!_stricmp(typeStr, formTypes[3 * i]))
			{
				filterTypeID = static_cast<unsigned char>(i);
				filterType = 4; // set to OTHER FORMS only
				break;
			}
		}
	}
	*(DWORD*)typeStr = filterType;
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(0x20);
	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_call<5>(ExtractArgsHookAddr, UInt64(OnHelpExtractArgs));
	trampoline.write_branch<5>(TypeIDHookAddr, UInt64(HookCheckTypeID));

	// allow two strings instead of string + int
	REL::safe_write(ParamInfoToReplaceAddr, TwoOptionalStringsAddr);

	// remove useless 'VIEWER STRINGS NOT AVAILABLE' print
	REL::safe_write(ViewerStringsAddr, "\x0f\x1f\x44\x00\x00", 5);

	// remove prints about help command usage
	REL::safe_write(HelpUsageAddr, "\xEB\x2A", 2);

	return true;
}
