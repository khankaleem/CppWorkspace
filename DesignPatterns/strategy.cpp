#include<iostream>
#include<string>
#include <sstream>
#include<vector>
#include<memory>

enum class OUTPUT_FORMAT : uint8_t {
    HTML = 0,
    MARKDOWN = 1
};

class ListStrategy {
  public:
    ListStrategy() = default;
    virtual ~ListStrategy() = default;
    virtual void start(std::ostringstream& oss) {}
    virtual void add_item(std::ostringstream& oss, const std::string& item_) {}
    virtual void end(std::ostringstream& oss) {}
};

class HtmlListStrategy : public ListStrategy {
  public:
    HtmlListStrategy() = default;
    virtual void start(std::ostringstream& oss) override {
      oss << "<ui>\n";
    }
    virtual void add_item(std::ostringstream& oss, const std::string& item_) override {
      oss << " <li>" << item_ << "</li>\n";
    }
    virtual void end(std::ostringstream& oss) override {
      oss << "</ui>\n";
    }
};

class MarkDownListStrategy : public ListStrategy {
  public:
    MarkDownListStrategy() = default;
    virtual void add_item(std::ostringstream& oss, const std::string& item_) override {
      oss << "* " << item_ << '\n';
    }
};

class TextProcessor {
  std::ostringstream m_oss;
  std::unique_ptr<ListStrategy> m_list;

public:
  TextProcessor() = default;

  void set_output_format(OUTPUT_FORMAT format_) {
    switch(format_) {
      case OUTPUT_FORMAT::HTML:
        m_list = std::make_unique<HtmlListStrategy>();
        break;
      case OUTPUT_FORMAT::MARKDOWN:
        m_list = std::make_unique<MarkDownListStrategy>();
        break;      
    }
  }

  void append_list(const std::vector<std::string>& list_) {
    m_list->start(m_oss);
    for (const auto& item: list_) {
      m_list->add_item(m_oss, item);
    }
    m_list->end(m_oss);
  }

  std::string str() const {
    return m_oss.str();
  }

  void clear() {
    m_oss.str("");
  }
};

int main() {
  std::vector<std::string> list = {"foo", "bar", "kaleem"};
  TextProcessor tp;

  tp.set_output_format(OUTPUT_FORMAT::MARKDOWN);
  tp.append_list(list);
  std::cout << tp.str() << '\n';
  tp.clear();

  tp.set_output_format(OUTPUT_FORMAT::HTML);
  tp.append_list(list);
  std::cout << tp.str() << '\n';
  tp.clear();
}








