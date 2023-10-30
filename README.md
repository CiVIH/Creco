# Creco - The Civilization Recognizer
**Creco** stands for "Civilization Recognizer". It's a specialized library designed to identify the currently played Civilization and its Leader when starting a game in *Civilization 6*. To achieve this, Creco is injected into the game where it launches a lightweight HTTP RESTful server, allowing you to retrieve the currently played Leader.

#### Key Features

- Seamlessly identifies the currently played Civilization and Leader.
- Lightweight HTTP RESTful server embedded.
- Only binds locally for security.
- Default server port: `45054`.
  
#### How it Works

Creco employs two sophisticated techniques known as **Hooking/Detouring** and **Signature Pattern Scanning**:

1. **Hooking/Detouring**: 
   - This technique involves intercepting calls to specific functions, redirecting the flow to our own functions, and optionally resuming the original function's execution. It's a way to "eavesdrop" or even "modify" the behavior of these functions at runtime.
   - In Creco's context, it hooks into two Lua functions used by the game.
   - The primary hooking happens in the Lua function `Local:StringDictionary:HasKey`. During the game's loading screen, a string constructed as "LOC_LOADING_INFO_<LeaderType>" is passed to this function. Post this call, Creco strips "LOC_LOADING_INFO_" to extract the LeaderType.
   - To ensure that we only detect during the loading screen, Creco also hooks into `UI.PlaySound`. It checks if the sound being played is "Play_DawnOfMan_Speech", which is unique to the loading screen.

2. **Signature Pattern Scanning**:
   - Rather than looking for explicit memory addresses, signature pattern scanning searches the process's memory for a specific byte-pattern. It offers the flexibility of detecting functions even if their addresses change, like after a game update.
   - With the help of IDA Pro, the function signatures are reverse-engineered and stored.
   - Using these signatures/patterns, Creco can dynamically locate the correct memory address of specific game functions on the fly. This means even if the game updates and changes its memory layout, Creco doesn't need manual adjustments to locate the functions, saving time and ensuring longevity.

#### System Requirements

- **OS**: Windows (x64 only).
- **Game**: Civilization 6.

#### Building Creco

1. Ensure you have Visual Studio 2022 installed.
2. Clone the Creco repository.
3. Open the solution/project file in Visual Studio.
4. Build the project following the standard Visual Studio build process.

#### Concluding Note

Creco aims to provide a seamless experience for *Civilization 6* enthusiasts, bridging the gap between the game and external tools that can leverage the rich information Creco extracts.