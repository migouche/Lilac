//
// Created by migouche on 11/14/2023.
//
#include <iostream>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <clang/Frontend/CompilerInstance.h>
#include "compiler/compiler.h"

#include <llvm/IR/Module.h>

#include "parser/parser.h"
#include "parser/parser_data.h"
#include "compiler/llvm-primitives/llvmops.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/IR/LegacyPassManager.h"
//#include "clang/Lex/PreprocessorOptions.h"
// #include <clang/CodeGen/CodeGenAction.h>


void Compiler::compile_file(const std::string& file,  const std::unique_ptr<ParserData>& parser_data) {
    Parser parser(file);
    trees.push_back(parser.get_tree());

}

ASTTree &Compiler::get_tree(size_t i) {
    return trees.at(i);
}

std::string Compiler::status() {
    return "hey ho";
}

Compiler::Compiler(const std::vector <std::string>& files, const std::string& output, int O, bool debug) {
    //std::shared_ptr parser_data = std::make_shared<parser_data::ParserData>();
    data = std::make_unique<ParserData>();

    LLVMOps::init(data);

    for(const auto& f: files)
        compile_file(f, data);


    for(const auto& t: trees)
    {
        t.codegen(data);
    }

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    auto target_triple = llvm::sys::getDefaultTargetTriple();

    std::string error;
    llvm::StringRef target_triple_ref(target_triple);

    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple_ref, error);
    if (!target) {
        llvm::errs() << error;
        return;
    }

    llvm::TargetOptions opt;

    auto rm = std::optional<llvm::Reloc::Model>();
    const std::unique_ptr<llvm::TargetMachine> targetMachine (target->createTargetMachine(target_triple, "generic", "", opt, rm));

    // mem input stream

    data->module->setDataLayout(targetMachine->createDataLayout());


    std::string output_object_file = "output.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(output_object_file, EC, llvm::sys::fs::OF_None);
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }
    llvm ::legacy::PassManager pass;

    if (auto ft = llvm::CodeGenFileType::ObjectFile; targetMachine->addPassesToEmitFile(pass, dest, nullptr, ft)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*data->module);
    dest.flush();
    llvm::outs() << "Wrote " << output_object_file << "\n";

    // Link the object file to create an executable
    std::string executable_file = "output";
    std::string link_command = "clang++ output.o -o " + executable_file;
    if (int link_result = std::system(link_command.c_str()); link_result != 0) {
        llvm::errs() << "Linking failed with exit code " << link_result << "\n";
    } else {
        llvm::outs() << "Executable created: " << executable_file << "\n";
        // delete the object file after linking
        std::string delete_command = "rm output.o";
        //std::system(delete_command.c_str());

    }


}

const std::unique_ptr<ParserData> & Compiler::get_parser_data() const{
    return data;
}


