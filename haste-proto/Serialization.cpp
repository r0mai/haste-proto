#include "Serialization.h"

namespace r0 {

WriterVisitor::StateGuard::StateGuard(WriterVisitor& writer)
	: writer_(writer)
	, current_(writer.current_) {}


WriterVisitor::StateGuard::~StateGuard() {
	writer_.current_ = current_;
}

WriterVisitor::WriterVisitor() {}

nlohmann::json& WriterVisitor::Current() {
	return *current_;
}

void WriterVisitor::VisitValue(int value) {
	Current() = value;
}

void WriterVisitor::VisitValue(const std::string& value) {
	Current() = value;
}

ReaderVisitor::StateGuard::StateGuard(ReaderVisitor& reader)
	: reader_(reader)
	, current_(reader.current_) {}


ReaderVisitor::StateGuard::~StateGuard() {
	reader_.current_ = current_;
}

ReaderVisitor::ReaderVisitor(const nlohmann::json& root) : root_(root) {}

const nlohmann::json& ReaderVisitor::Current() {
	return *current_;
}

void ReaderVisitor::ReportError(const char* error) {
	tfm::printfln(error);
	hasError_ = true;
}

void ReaderVisitor::VisitValue(int& value) {
	if (!Current().is_number_integer()) {
		ReportError("Reader: not an integer");
		return;
	}

	Current().get_to(value);
}

void ReaderVisitor::VisitValue(std::string& value) {
	if (!Current().is_string()) {
		ReportError("Reader: not a string");
		return;
	}

	Current().get_to(value);
}

} // namespace r0