Submodules
===========

Git submodules are used in this project. They contain utility and third party source code and binaries. Depending on what you are doing, you may not need all the submodules. When the repository is initially cloned the submodule directories will not be populated with code/files. To populate them you need to issue the "git submodule init" and "git submodule update" commands to initialise and populate the submodules with the (correct commit/version) of each repository. For example,

```bash script
>> cd myrepos/rjmcmcmt
>> git submodule init 
>> git submodule update 
>> git submodule status 
```

# cpp-utils
	- C++ utility code, classes and functions used in a variety of separate repositories.
	- Author: Ross C Brodie
	- Language: C++
	- Repository: git@github.com:rcb547/cpp-utils.git
	- Only required if you want to compile the source code.
	- Not required if you just want to run the precompiled Windows executables.

