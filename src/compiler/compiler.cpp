//
// Created by migouche on 11/14/2023.
//
#include <iostream>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/TargetSelect.h>
#include "compiler/compiler.h"
#include "parser/parser.h"
#include "parser/parser_data.h"
#include "compiler/llvm-primitives/llvmops.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/IR/LegacyPassManager.h"

void Compiler::compile_file(const std::string& file,  const std::shared_ptr<ParserData>& parser_data) {
    Parser parser(file);
    trees.push_back(parser.get_tree());

}

ASTTree &Compiler::get_tree(size_t i) {
    return trees.at(i);
}

std::string Compiler::status() {
    return "hey ho";
}

Compiler::Compiler(const std::vector <std::string>& files) {
    //std::shared_ptr parser_data = std::make_shared<parser_data::ParserData>();
    data = std::make_shared<ParserData>();

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

    auto cpu = "generic";
    auto features = "";
    llvm::TargetOptions opt;


    auto rm = std::optional<llvm::Reloc::Model>();
    llvm::TargetMachine *targetMachine = target->createTargetMachine(target_triple, cpu, features, opt, rm);

    data->module->setDataLayout(targetMachine->createDataLayout());
    std::string output_name = "output.out";

    std::error_code EC;
    llvm::raw_fd_ostream dest(output_name, EC, llvm::sys::fs::OF_None);
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }

    llvm::legacy::PassManager pass;
    llvm::CodeGenFileType ft = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, ft)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*data->module);
    dest.flush();
    llvm::outs() << "Wrote " << output_name << "\n";

}

std::shared_ptr<ParserData> Compiler::get_parser_data() {
    return data;
}


