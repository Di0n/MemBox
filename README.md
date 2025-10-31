# MemBox
A library for patching memory

## Usage

### Using the Process class

```
using namespace MemBox;

std::expected<Process, MemboxException> proc = Process::Attach(PROCESS_ALL_ACCESS, 14567);
// Or
std::expected<Process, MemboxException> proc = Process::Attach(PROCESS_ALL_ACCESS, L"MyProgram.exe");

if (!proc.has_value())
{
    std::print("Could not attach process, {}", proc.error().what());
    return 0;
}

```

To attach to a process you will need to supply 
## Libraries used
* [utf8cpp](https://github.com/nemtrif/utfcpp)
