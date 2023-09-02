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

} // namespace r0