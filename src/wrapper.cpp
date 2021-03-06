#include <algorithm>
#include <iostream>
#include "wrapper.h"
#include "tools.h"
#include "shareasale_record.h"
#include "prestashop_record.h"

namespace fc
{

Wrapper::Wrapper(const char in_sep, const char out_sep, const std::string& category, 
    		const std::vector<std::string>& matching_w, const std::vector<std::string>& excluded_w, double min_price) :
  _in_sep(in_sep), _out_sep(out_sep), _category(category), mw{matching_w}, xw{excluded_w}, _min_price(min_price)
{
}

bool Wrapper::wrap(const std::string& in_str, std::string& out_str)
{
  Shareasale_record in_rec(in_str, _in_sep);
  double price = atof(in_rec.at(7).c_str());
  std::string desc{in_rec.at(1)};
  std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
  if (match(desc, price)) {
    Prestashop_record out_rec(_out_sep);
    wrap(in_rec, out_rec);
    out_str = out_rec.asString();
    return true;
  }
  else {
    return false;
  }
}

void Wrapper::wrap(const Shareasale_record& in_record, Prestashop_record& out_record)
{
  int i=0;
  out_record.setField(i++, in_record.at(0));
  out_record.setField(i++, "1");
  out_record.setField(i++, in_record.at(1));
  out_record.setField(i++, _category);
  out_record.setField(i++, in_record.at(7));
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "1");
  out_record.setField(i++, "");
  double old_price = atof(in_record.at(8).c_str());
  if (old_price == 0.0)
    out_record.setField(i++, "0");
  else {
    double new_price = atof(in_record.at(7).c_str());
    double reduction = (old_price - new_price) / old_price;
    out_record.setField(i++, std::to_string(static_cast<int>(reduction)));
  }
  out_record.setField(i++, "2000-01-01");
  out_record.setField(i++, "2099-12-31");
  out_record.setField(i++, in_record.at(0));
  out_record.setField(i++, in_record.at(20));
  out_record.setField(i++, in_record.at(2));
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "99");
  out_record.setField(i++, "1");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, in_record.at(1));
  out_record.setField(i++, in_record.at(11));
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "in stock");
  out_record.setField(i++, "");
  out_record.setField(i++, "1");
  out_record.setField(i++, "2000-01-01");
  out_record.setField(i++, "2000-01-01");
  out_record.setField(i++, "1");
  out_record.setField(i++, in_record.at(6));
  out_record.setField(i++, "1");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
  out_record.setField(i++, "0");
}

bool Wrapper::match(const std::string& s, double price)
{
  if (price < _min_price) {
    Tools::log(E_level::DEBUG, "DO_NOT_MATCH", "price " + std::to_string(price) + " is too low");
    return false;
  }

  bool found_to_exclude = doMatch(s, xw, false);
  if (found_to_exclude) {
    Tools::log(E_level::DEBUG, "DO_NOT_MATCH", "found word to exclude in : " + s);
    return false;
  }
  else {
    bool found_to_include = doMatch(s, mw, true);
    if (found_to_include)
      Tools::log(E_level::DEBUG, "DO_MATCH", "found word to include in : " + s);
    else
      Tools::log(E_level::DEBUG, "DO_NOT_MATCH", "did not found word to include in : " + s);
    return found_to_include;
  }
}

bool Wrapper::doMatch(const std::string& s, const std::vector<std::string>& v, bool default_found)
{
  bool found{false};
  if (v.size() > 0) {
    for (auto& word : v) {
      std::vector<std::string> v2 = Tools::getCompleteWord(word);
      for (auto& word2 : v2) {
	size_t pos = s.find(word2);
	found |= (pos != std::string::npos);
      }
    }
  }
  else {
    found = default_found;
  }
  return found;
}

Wrapper::~Wrapper()
{
}

} // end namespace fc

