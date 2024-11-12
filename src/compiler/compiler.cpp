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
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Job.h>
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

void write_object_file_to_disk(const std::unique_ptr<ParserData>& data, const std::string& output) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    std::string target_triple = llvm::sys::getDefaultTargetTriple();
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if(!target)
    {
        llvm::errs() << error;
        return;
    }

    llvm::TargetOptions opt;
    std::unique_ptr<llvm::TargetMachine> target_machine(target->createTargetMachine(target_triple, "generic", "", opt, llvm::Reloc::PIC_));

    data->module->setDataLayout(target_machine->createDataLayout());
    data->module->setTargetTriple(target_triple);

    llvm::SmallVector<char, 0> buffer;
    llvm::raw_svector_ostream dest(buffer);

    llvm::legacy::PassManager pass;
    if(target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile))
    {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return;
    }else
    {
        pass.run(*data->module);
        std::ofstream out(output, std::ios::binary);
        out.write(buffer.data(), buffer.size());
    }
}

std::unique_ptr<llvm::MemoryBuffer> generate_object_file(const std::unique_ptr<ParserData>& data)
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    std::string target_triple = llvm::sys::getDefaultTargetTriple();
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if(!target)
    {
        llvm::errs() << error;
        return nullptr;
    }

    llvm::TargetOptions opt;
    std::unique_ptr<llvm::TargetMachine> target_machine(target->createTargetMachine(target_triple, "generic", "", opt, llvm::Reloc::PIC_));

    data->module->setDataLayout(target_machine->createDataLayout());
    data->module->setTargetTriple(target_triple);

    llvm::SmallVector<char, 0> buffer;
    llvm::raw_svector_ostream dest(buffer);

    llvm::legacy::PassManager pass;
    if(target_machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile))
    {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return nullptr;
    }else
    {
        pass.run(*data->module);
        //dest.flush();
        return llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(buffer.data(), buffer.size()));
    }

}

void clang_compile(const std::unique_ptr<llvm::MemoryBuffer>& obj_file, const std::string& output) {
    clang::CompilerInstance compiler;
    compiler.createDiagnostics();

    std::shared_ptr<clang::TextDiagnosticPrinter> diag_printer = std::make_shared<clang::TextDiagnosticPrinter>(llvm::errs(), &compiler.getDiagnosticOpts());
    compiler.getDiagnostics().setClient(diag_printer.get(), false);

    clang::CompilerInvocation::CreateFromArgs(compiler.getInvocation(), {"clang", "-o", output.c_str(), "-x", "ir", "-"}, compiler.getDiagnostics());

    compiler.createFileManager();
    compiler.createSourceManager(compiler.getFileManager());

    if(!obj_file)
    {
        llvm::errs() << "Error generating object file\n";
        return;
    }
    std::unique_ptr<llvm::MemoryBuffer> buffer_ptr = llvm::MemoryBuffer::getMemBufferCopy(obj_file->getBuffer(), "input.o");

    if (!buffer_ptr) {
        llvm::errs() << "Error creating memory buffer\n";
        return;
    }

    compiler.getSourceManager().setMainFileID(compiler.getSourceManager().createFileID(std::move(buffer_ptr)));

    compiler.createPreprocessor(clang::TU_Complete);
    compiler.createASTContext();

    clang::EmitObjAction action;
    if (!compiler.ExecuteAction(action)) {
        llvm::errs() << "Error compiling object file\n";
        return;
    }

    llvm::outs() << "Wrote " << output << "\n";
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

    //auto obj = generate_object_file(data);
    //clang_compile(obj, "exec.out");

    write_object_file_to_disk(data, "exec.o");

}


const std::unique_ptr<ParserData> & Compiler::get_parser_data() const{
    return data;
}


