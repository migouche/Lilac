//
// Created by migouche on 11/14/2023.
//
#include <iostream>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <clang/Frontend/CompilerInstance.h>
#include "compiler/compiler.h"
#include "parser/parser.h"
#include "parser/parser_data.h"
#include "compiler/llvm-primitives/llvmops.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/IR/LegacyPassManager.h"
#include "clang/Lex/PreprocessorOptions.h"
 #include <clang/CodeGen/CodeGenAction.h>
#include <fstream>


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

Compiler::Compiler(const std::vector <std::string>& files) {
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
    std::unique_ptr<llvm::TargetMachine> targetMachine(target->createTargetMachine(target_triple, "generic", "", opt, rm));
    data->module->setDataLayout(targetMachine->createDataLayout());

    llvm::SmallVector<char, 0> buffer;
    llvm::raw_svector_ostream dest(buffer);

    llvm::legacy::PassManager pass;
    llvm::CodeGenFileType ft = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, ft)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*data->module);
    llvm::outs() << "Wrote " << buffer.size() << " bytes\n";

    std::string obj_file = "output.o";
    std::ofstream obj_stream(obj_file, std::ios::binary);
    obj_stream.write(buffer.data(), buffer.size());
    obj_stream.close();

    std::string output_name = "executable.out";
    std::string command = "clang++ " + obj_file + " -o " + output_name;
    if (system(command.c_str()) != 0) {
        llvm::errs() << "Error: could not link object file\n";
        return;
    }

    llvm::outs() << "Wrote " << output_name << "\n";


}


const std::unique_ptr<ParserData> & Compiler::get_parser_data() const{
    return data;
}


