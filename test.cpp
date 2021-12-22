INT64(__fastcall* Qword_ptrOriginal)(PVOID, PVOID, PVOID, PVOID, PVOID);

INT64 __fastcall NtExample(PVOID a1, PVOID a2, PVOID SectionInfo, PVOID a4, PVOID a5)
{

	return Qword_ptrOriginal(a1, a2, SectionInfo, a4, a5);
}

extern "C" NTSTATUS Main(PDRIVER_OBJECT DriverObj, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObj);
	UNREFERENCED_PARAMETER(RegistryPath);


	auto wink32base = mem::GetSystemBaseModule("\\SystemRoot\\System32\\win32kbase.sys");

	if (wink32base)
	{
		auto dataPtr = FindPattern((UINT64)wink32base, (UINT64)0xFFFFFFFFFF, (BYTE*)"example sig", "mask");

		if (dataPtr)
		{
			UINT64 qword_deref = (UINT64)(dataPtr);

			qword_deref = (UINT64)qword_ptr_derf + *(PINT)((PBYTE)qword_ptr_derf + 3) + 7; //6

			auto RVA = qword_ptr_derf - (UINT64)wink32base;

			PEPROCESS Target;
			NTSTATUS Status;

			if (NT_SUCCESS(Status = FindProcess("explorer.exe", &Target)))
			{
				if (Target)
				{
					KeAttachProcess(Target);

					*(PVOID*)&Qword_ptrOriginal = InterlockedExchangePointer((PVOID*)qword_deref, (PVOID)NtExample);

					KeDetachProcess();
				}
				else
				{
					Printf("Error!");
				}
			}
			else
			{
				Printf("Error! explorer.exe not found! Status : 0x%x", Status);
			}


		}
		else
		{
			Printf("Error!\n");
		}
	}
	else
	{
		Printf("Error!\n");

	}


	Printf("hook\n");
	return STATUS_SUCCESS;
}
