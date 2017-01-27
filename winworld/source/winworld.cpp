#include "winworld.h"

MarkovChain *markov = NULL;

#ifdef _WIN32
# include <Processthreadsapi.h>

#ifdef SHOW_MITIGATIONS
static void show_mitigations(void)
{
	HANDLE hProc = GetCurrentProcess();
	PROCESS_MITIGATION_ASLR_POLICY aslr = { 0 };
	PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY strict_handle_check = { 0 };
	PROCESS_MITIGATION_DYNAMIC_CODE_POLICY dynamic_code = { 0 };
	PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY system_call_disable = { 0 };
	PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY extension_point_disable = { 0 };
	PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY cfg = { 0 };
	PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY signature = { 0 };
	PROCESS_MITIGATION_FONT_DISABLE_POLICY font = { 0 };
	PROCESS_MITIGATION_IMAGE_LOAD_POLICY image_load = { 0 };

	GetProcessMitigationPolicy(hProc, ProcessASLRPolicy, &aslr, sizeof(aslr));
	printf("ProcessASLRPolicy:\n");
	printf("   EnableBottomUpRandomization                %u\n", aslr.EnableBottomUpRandomization);
	printf("   EnableForceRelocateImages                  %u\n", aslr.EnableForceRelocateImages);
	printf("   EnableHighEntropy                          %u\n", aslr.EnableHighEntropy);
	printf("   DisallowStrippedImages                     %u\n", aslr.DisallowStrippedImages);

	GetProcessMitigationPolicy(hProc, ProcessDynamicCodePolicy, &dynamic_code, sizeof(dynamic_code));
	printf("ProcessStrictHandleCheckPolicy:\n");
	printf("   ProhibitDynamicCode                        %u\n", dynamic_code.ProhibitDynamicCode);

	GetProcessMitigationPolicy(hProc, ProcessStrictHandleCheckPolicy, &strict_handle_check, sizeof(strict_handle_check));
	printf("ProcessStrictHandleCheckPolicy:\n");
	printf("   RaiseExceptionOnInvalidHandleReference     %u\n", strict_handle_check.RaiseExceptionOnInvalidHandleReference);
	printf("   HandleExceptionsPermanentlyEnabled         %u\n", strict_handle_check.HandleExceptionsPermanentlyEnabled);

	GetProcessMitigationPolicy(hProc, ProcessSystemCallDisablePolicy, &system_call_disable, sizeof(system_call_disable));
	printf("ProcessSystemCallDisablePolicy:\n");
	printf("   DisallowWin32kSystemCalls                  %u\n", system_call_disable.DisallowWin32kSystemCalls);

	GetProcessMitigationPolicy(hProc, ProcessExtensionPointDisablePolicy, &extension_point_disable, sizeof(extension_point_disable));
	printf("ProcessExtensionPointDisablePolicy:\n");
	printf("   DisableExtensionPoints                     %u\n", extension_point_disable.DisableExtensionPoints);

	GetProcessMitigationPolicy(hProc, ProcessControlFlowGuardPolicy, &cfg, sizeof(cfg));
	printf("ProcessControlFlowGuardPolicy:\n");
	printf("   EnableControlFlowGuard                     %u\n", cfg.EnableControlFlowGuard);

	GetProcessMitigationPolicy(hProc, ProcessSignaturePolicy, &signature, sizeof(signature));
	printf("ProcessSignaturePolicy:\n");
	printf("   MicrosoftSignedOnly                        %u\n", signature.MicrosoftSignedOnly);

	GetProcessMitigationPolicy(hProc, ProcessFontDisablePolicy, &font, sizeof(font));
	printf("ProcessFontDisablePolicy:\n");
	printf("   DisableNonSystemFonts                      %u\n", font.DisableNonSystemFonts);

	GetProcessMitigationPolicy(hProc, ProcessImageLoadPolicy, &image_load, sizeof(image_load));
	printf("ProcessImageLoadPolicy:\n");
	printf("   NoRemoteImages                             %u\n", image_load.NoRemoteImages);
	printf("   NoLowMandatoryLabelImages                  %u\n", image_load.NoLowMandatoryLabelImages);
}
#endif

static void secure_park(void)
{
	HANDLE hProc = GetCurrentProcess();
	PROCESS_MITIGATION_DEP_POLICY dep = { 0 };
	PROCESS_MITIGATION_ASLR_POLICY aslr = { 0 };
	PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY strict_handle_check = { 0 };
	PROCESS_MITIGATION_DYNAMIC_CODE_POLICY dynamic_code = { 0 };
	PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY system_call_disable = { 0 };
	PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY extension_point_disable = { 0 };
	PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY cfg = { 0 };
	PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY signature = { 0 };
	PROCESS_MITIGATION_FONT_DISABLE_POLICY font = { 0 };
	PROCESS_MITIGATION_IMAGE_LOAD_POLICY image_load = { 0 };

	// ASLR Policy

	GetProcessMitigationPolicy(hProc, ProcessASLRPolicy, &aslr, sizeof(aslr));
	aslr.EnableBottomUpRandomization = 1;
	aslr.EnableForceRelocateImages = 1;
	aslr.EnableHighEntropy = 1;
	aslr.DisallowStrippedImages = 1;

	if (!SetProcessMitigationPolicy(ProcessASLRPolicy, &aslr, sizeof(aslr))) {
#ifdef DEBUG
		std::cerr << "Failed to set ASLR Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// Dynamic Code Policy

	GetProcessMitigationPolicy(hProc, ProcessDynamicCodePolicy, &dynamic_code, sizeof(dynamic_code));
	dynamic_code.ProhibitDynamicCode = 1;
 
	if (!SetProcessMitigationPolicy(ProcessDynamicCodePolicy, &dynamic_code, sizeof(dynamic_code))) {
#ifdef DEBUG
		std::cerr << "Failed to set Dynamic Code Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// Strict Handle Check Policy

	GetProcessMitigationPolicy(hProc, ProcessStrictHandleCheckPolicy, &strict_handle_check, sizeof(strict_handle_check));
	strict_handle_check.RaiseExceptionOnInvalidHandleReference = 1;
	strict_handle_check.HandleExceptionsPermanentlyEnabled = 1;

	if (!SetProcessMitigationPolicy(ProcessStrictHandleCheckPolicy, &strict_handle_check, sizeof(strict_handle_check))) {
#ifdef DEBUG
		std::cerr << "Failed to set Strict Handle Check Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// System Call Disable Policy

	GetProcessMitigationPolicy(hProc, ProcessSystemCallDisablePolicy, &system_call_disable, sizeof(system_call_disable));
	system_call_disable.DisallowWin32kSystemCalls = 1;

	if (!SetProcessMitigationPolicy(ProcessSystemCallDisablePolicy, &system_call_disable, sizeof(system_call_disable))) {
#ifdef DEBUG
		std::cerr << "Failed to set System Call Disable Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// Extension Point Disable Policy

	GetProcessMitigationPolicy(hProc, ProcessExtensionPointDisablePolicy, &extension_point_disable, sizeof(extension_point_disable));
	extension_point_disable.DisableExtensionPoints = 1;

	if (!SetProcessMitigationPolicy(ProcessExtensionPointDisablePolicy, &extension_point_disable, sizeof(extension_point_disable))) {
#ifdef DEBUG
		std::cerr << "Failed to set Extension Point Disable Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// Control Flow Guard Policy

	GetProcessMitigationPolicy(hProc, ProcessControlFlowGuardPolicy, &cfg, sizeof(cfg));
	cfg.EnableControlFlowGuard = 1;

	if (!SetProcessMitigationPolicy(ProcessControlFlowGuardPolicy, &cfg, sizeof(cfg))) {
#ifdef DEBUG
		std::cerr << "Failed to set Control Flow Guard Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// Signature Policy

	GetProcessMitigationPolicy(hProc, ProcessSignaturePolicy, &signature, sizeof(signature));
	signature.MicrosoftSignedOnly = 1;

	if (!SetProcessMitigationPolicy(ProcessSignaturePolicy, &signature, sizeof(signature))) {
#ifdef DEBUG
		std::cerr << "Failed to set Signature Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// Font Disable Policy

	GetProcessMitigationPolicy(hProc, ProcessFontDisablePolicy, &font, sizeof(font));
	font.DisableNonSystemFonts = true;

	signature.MicrosoftSignedOnly = 1;
	if (!SetProcessMitigationPolicy(ProcessFontDisablePolicy, &font, sizeof(font))) {
#ifdef DEBUG
		std::cerr << "Failed to set Font Disable Policy (" << GetLastError() << ")" << std::endl;
#endif
	}

	// Image Load Policy
	GetProcessMitigationPolicy(hProc, ProcessImageLoadPolicy, &image_load, sizeof(image_load));
	image_load.NoRemoteImages = 1;
	image_load.NoLowMandatoryLabelImages = 1;

	if (!SetProcessMitigationPolicy(ProcessImageLoadPolicy, &image_load, sizeof(image_load))) {
#ifdef DEBUG
		std::cerr << "Failed to set Image Load Policy (" << GetLastError() << ")" << std::endl;
#endif
	}
#ifdef SHOW_MITIGATIONS
	show_mitigations();
#endif
}
#endif

void init_park(std::unique_ptr<Narrator>& narrator)
{
	std::cout.setstate(std::ios::failbit);
	person_t dolores = narrator->newPerson(HOST, FEMALE, "Dolores");
	person_t maeve = narrator->newPerson(HOST, FEMALE, "Maeve");
	person_t clementine = narrator->newPerson(HOST, FEMALE, "Clementine");
	person_t teddy = narrator->newPerson(HOST, MALE, "Teddy");
	person_t armistice = narrator->newPerson(HOST, FEMALE, "Armistice");
	person_t hector = narrator->newPerson(HOST, MALE, "Hector");
	person_t lawrence = narrator->newPerson(HOST, MALE, "Lawrence");

	person_t blackhat = narrator->newPerson(GUEST, MALE, "The Man in Black");
	person_t william = narrator->newPerson(GUEST, MALE, "William");
	person_t logan = narrator->newPerson(GUEST, MALE, "Logan");

	init_person(dolores);
	init_person(maeve);
	init_person(clementine);
	init_person(teddy);
	init_person(armistice);
	init_person(hector);
	init_person(lawrence);
	init_person(blackhat);
	init_person(william);
	init_person(logan);

	add_friend(dolores, teddy);
	add_friend(dolores, william);
	add_friend(maeve, clementine);
	add_friend(maeve, logan);
	add_friend(clementine, logan);
	add_friend(teddy, william);
	add_friend(armistice, hector);
	add_friend(lawrence, william);
	add_friend(lawrence, logan);
	add_friend(william, logan);

	std::cout.clear();
}

int main()
{
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

#ifdef _WIN32
	secure_park();
#endif

	srand((unsigned int)time(NULL));
	std::cout << "--[ Welcome to Winworld, park no " << rand() %  1337 << " ]--" << std::endl;

	std::string line;
	std::unique_ptr<Narrator> narrator(new Narrator);
	markov = new MarkovChain;

	markov->generateFromFile("westworld.txt");

	init_park(narrator);

	std::cout << "narrator [day " << narrator->getDays() << "]$ ";
	bool hide_prompt = false;

	while (std::getline(std::cin, line)) {
		if (!line.compare("prompt hide")) {
			hide_prompt = true;
			continue;
		} else if (!line.compare("prompt show")) {
			hide_prompt = false;
			continue;
		}
		if (!narrator->executeCommand(line)) {
			break;
		}

		if (!hide_prompt) {
			std::cout << "narrator [day " << narrator->getDays() << "]$ ";
		}
	}
}