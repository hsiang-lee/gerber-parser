#ifndef GLOBAL_DATA_MACHINEID_H
#define GLOBAL_DATA_MACHINEID_H

// for key length
enum class KEYLEN {
    kKeyLength64 = 64,
    kKeyLength128 = 128,
    kKeyLength256 = 256,
    kKeyLength512 = 512,
    kKeyLength1024 = 1024
};

// for file operation type
enum class OPTYPE {
    kKey = 1,
    kAuth,
    kEnca,
    kDeca,
    kOther
};

// for return status
// "authorization file is incomplete" refers to -4
// "machine id mismacth" refers to -3
// "effective date pass" refers to -2
// "usage exceede" refers to -1
// "success" refers to 0
enum class RETURNSTATUS {
    kAuthFileIncomplete = -4,
    kMachineIdMismatch,
    kEffectiveDatePass,
    kUsageExceede,
    kSuccess
};

// for register return value
// Could not open registry key
// Could not read registry value
// Incorrect registry value type
// Success
enum class REGRETURNSTATUS {
    kRegKeyOpenFail = -3,
    kRegKeyReadFail,
    kRegKeyValueIncorrect,
    kSuccess
};

#endif
