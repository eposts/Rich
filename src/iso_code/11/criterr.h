

char *criterr_screen[] = {
"ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿",
"³                  tde critical error handler                       ³",
"³                                                                   ³",
"³    Error code:                                                    ³",
"³     Operation:                                                    ³",
"³         Drive:                                                    ³",
"³ Extended Code:                                                    ³",
"³   Error Class:                                                    ³",
"³         Locus:                                                    ³",
"³   Device Type:                                                    ³",
"³   Device Name:                                                    ³",
"³                                                                   ³",
"³            Please enter action:  (Q)uit or (R)etry?               ³",
"³               (ONLY AS A LAST RESORT -- (A)bort)                  ³",
"ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ",
NULL
};



char *error_code[] = {
   "Attempt to write on write-protected disk",    
   "Unknown unit",
   "Drive not ready",
   "Unknown command",
   "Data error (CRC)",
   "Bad drive request structure length",          
   "Seek error",
   "Unknown media type",
   "Sector not found",
   "Printer out of paper",
   "Write fault",                                 
   "Read fault",
   "General failure"
};



char *operation[] = {
   "Read",
   "Write"
};



char *error_class[] = {
   "Unknown",
   "Out of Resource:  space, channels, etc...",         
   "Temporary Situation:  expected to end",
   "Authorization: permission problem",
   "Internal: error in system software",
   "Hardware Failure:  not the fault of tde",           
   "System Failure:  system software",
   "Application Program Error",
   "Not Found:  file/item not found",
   "Bad Format:  invalid format, type",
   "Locked:  file* 10 */
   "Media:  wrong disk, bad spot, etc...",
   "Already Exists:  collision with existing",
   "Unknown"
};


char *locus[] = {
   "Unknown",
   "Unknown",
   "Block Device (disk or disk emulator)",
   "Network",
   "Serial Device:  keyboard, printer, plotter, modem",
   "Memory:  random access memory"
};



char *device_type[] = {
   "Block",
   "Character"
};



char *ext_err[] = {
   "Frank has no idea what this error is",              
   "Function number invalid",
   "File not found",
   "Path not found",
   "Too many open files (no handles left)",
   "Access denied",
   "Handle invalid",
   "Memory control blocks destroyed",
   "Insufficient memory",
   "Memory block address invalid",
   "Environment invalid",                               
   "Format invalid",
   "Access code invalid",
   "Data invalid",
   "Unknown unit",
   "Invalid drive was specified",
   "Attempt to remove current directory",
   "Not same device",
   "No more files",
   "Attempt to write on write-protected disk",
   "Unknown unit",                                      
   "Drive not ready",
   "Unknown command",
   "Data error (CRC)",
   "Bad request structure length",
   "Seek error",
   "Unknown media type",
   "Sector not found",
   "Printer out of paper",
   "Write fault",
   "Read fault",                                        
   "General failure",
   "Sharing violation",
   "File-lock voilation",
   "Invalid disk change",
   "FCB unavailable",
   "Sharing buffer overflow",
   "37",
   "38",
   "39",
   "40",
   "41",
   "42",
   "43",
   "44",
   "45",
   "46",
   "47",
   "48",
   "49",
   "Unsupported network request",
   "Remote machine not listening",
   "Duplicate name on network",
   "Network name not found",
   "Network busy",
   "Device no longer exists on network",
   "NetBIOS command limit exceeded",
   "Network adapter hardware failure",
   "Incorrect response from network",
   "Unexpected network error",
   "Incompatible remote adapter",                       
   "Print queue full",
   "Not enough room for print file",
   "Print file was deleted",
   "Network name deleted",
   "Network access denied",
   "Incorrect network device type",
   "Network name not found",
   "Network name limit exceeded",
   "Net BIOS session limit exceeded",
   "Temporarily paused",                                
   "Network request not accepted",
   "Print or disk redirection is paused",
   "73",
   "74",
   "75",
   "76",
   "77",
   "78",
   "79",
   "File already exists",                               
   "Reserved",
   "Cannot make directory entry",
   "Fail on INT 24",
   "Too many redirections",
   "Duplicate redirection",
   "Invalid password",
   "Invalid parameter",
   "Network device fault"                               
};
