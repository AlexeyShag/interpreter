llvm-profdata-10 merge -sparse build/default.profraw -o build/all.profdata
llvm-cov-10 report -instr-profile=build/all.profdata -object build/libinter.so
#llvm-cov-10 show -instr-profile=build/all.profdata -object build/libinter.so -format=html -output-dir coverage
