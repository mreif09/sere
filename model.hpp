#include <string>
#include <memory>
#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include <sstream>

static int id = 0;

class Identifiable {
 public:
  std::string desc;
  std::string identifier;

  std::string long_name;

  Identifiable() : identifier{std::to_string(id++)} {}

// public:
//  std::string desc() { return m_desc; }
//  void set_desc(const std::string& desc) { m_desc = desc; }
//
//  std::string identifier() { return m_identifier; }
//  void set_identifier(const std::string& id) { m_identifier = id; }
//
//  std::string long_name() { return m_long_name; }
//  void set_long_name(const std::string& long_name) { m_long_name= long_name; }
};

class AccessControlledElement : public Identifiable {
 public:
  bool is_editable;
};

class DatatypeDefinition: public Identifiable {
 public:
  virtual bool is_value_valid(const std::string&) const = 0;
};

class DatatypeDefinitionSimple: public DatatypeDefinition {};

class DatatypeDefinitionString: public DatatypeDefinitionSimple{
 public:
  int max_length;

  bool is_value_valid(const std::string& value) const override {
    return value.length() <= max_length;
  }
};

class DatatypeDefinitionInteger: public DatatypeDefinitionSimple{
 public:
  int min;
  int max;

  bool is_value_valid(const std::string& value) const override {
    std::stringstream value_stream(value);
    int x;
    value_stream >> x;
    return (x >= min) && (x <= max);
  }
};

class EnumValue : public Identifiable {
 public:
  const int key;
  const std::string content;

  EnumValue(int k, std::string v) : key{k}, content{v} {}
};

class DatatypeDefinitionEnumeration : public DatatypeDefinition {
 private:
  std::map<std::string, std::shared_ptr<EnumValue>> m_enum_values;

 public:
  auto& enum_values() { return m_enum_values; }

  bool is_value_valid(const std::string& value) const override {
    return std::any_of(m_enum_values.begin(), m_enum_values.end(), [&value](auto& element){ return value == element.second->content; });;
  };
};

class AttributeDefinition : public AccessControlledElement {
 public:
  virtual bool multi_valued() const = 0;
};

template<class T>
class AttributeDefinitionSimple : public AttributeDefinition {
private:
  std::shared_ptr<T> m_type;

 public:
  AttributeDefinitionSimple(std::shared_ptr<T> type)
      : m_type(type) {}

  bool multi_valued() const override { return false; };
  std::shared_ptr<T> type() { return m_type; }
};

class AttributeDefinitionString : public AttributeDefinitionSimple<DatatypeDefinitionString> {
 public:
  using AttributeDefinitionSimple::AttributeDefinitionSimple;
};

class AttributeDefinitionInteger : public AttributeDefinitionSimple<DatatypeDefinitionInteger> {
 public:
  using AttributeDefinitionSimple::AttributeDefinitionSimple;
};

class AttributeDefinitionEnumeration : public AttributeDefinition {
 private:
  bool m_multi_valued;
  std::shared_ptr<DatatypeDefinitionEnumeration> m_type;

 public:
  AttributeDefinitionEnumeration(std::shared_ptr<DatatypeDefinitionEnumeration> type)
      : m_type(type) {}

  void set_multi_valued(bool value) { m_multi_valued = value; }
  bool multi_valued() const override { return m_multi_valued; };

  std::shared_ptr<DatatypeDefinitionEnumeration> type() { return m_type; }
};

class AttributeValue {
 public:
  virtual bool set_value(const std::string&) = 0;
  virtual std::string value() const = 0;
  virtual std::vector<std::string> values() const = 0;
  virtual bool add_value(const std::string&) = 0;
  virtual void remove_value(const std::string&) = 0;
  virtual void clear_values() = 0;
};

template<class T>
class AttributeValueSimple : public AttributeValue {
 private:
  std::string m_value;
  std::shared_ptr<T> m_definition;

 public:
  AttributeValueSimple(std::shared_ptr<T> definition) : m_definition{definition} {}

  bool set_value(const std::string& value) override {
    if (m_definition->type()->is_value_valid(value) == false) return false;
    m_value = value;
    return true;
  }

  std::string value() const override { return m_value; }

  bool add_value(const std::string&) override { return false; }
  void remove_value(const std::string&) override {}
  void clear_values() override {}
  std::vector<std::string> values() const override {
    std::vector<std::string>  v;
    v.push_back(value());

    return v;
  }
};

class AttributeValueString : public AttributeValueSimple<AttributeDefinitionString> {
 public:
  using AttributeValueSimple::AttributeValueSimple;
};

class AttributeValueInteger : public AttributeValueSimple<AttributeDefinitionInteger> {
 public:
  using AttributeValueSimple::AttributeValueSimple;
};

class AttributeValueEnumeration : public AttributeValue {
 private:
  std::map<std::string, std::shared_ptr<EnumValue>> m_values;
  std::shared_ptr<AttributeDefinitionEnumeration> m_definition;

 public:
   AttributeValueEnumeration(std::shared_ptr<AttributeDefinitionEnumeration> definition)
	: m_definition{definition} {}

   bool add_value(const std::string& value) override {
     if (m_definition->type()->is_value_valid(value) == false) return false;
     auto type = m_definition->type();
     auto v = std::find_if(type->enum_values().begin(), type->enum_values().end(), [&value](auto& element){ return value == element.second->content; });
     m_values[v->first] = v->second;

     return true;
   }

  // Todo remove by value
  void remove_value(const std::string& key) override { m_values.erase(key); }
  void clear_values() override { m_values.clear(); }

  bool set_value(const std::string& value) override {
    return false;
  }

  std::vector<std::string> values() const override {
    std::vector<std::string>  v;
    for (auto&_value : m_values) {
      v.push_back(_value.second->content);
    }

    return v;
  }

  std::string value() const override { return ""; }
};

class AttributeValueFactoryBase {
 public:
  virtual std::unique_ptr<AttributeValue> create_value() const = 0;
};

template<class T>
class AttributeValueFactory : public AttributeValueFactoryBase {
 private:
  T m_default_value;

 public:
  T& default_value() { return m_default_value; };

    AttributeValueFactory(const T& default_value)
	: m_default_value{default_value} {}

  std::unique_ptr<AttributeValue> create_value() const override {
    return std::make_unique<T>(m_default_value);
  }
};


class SpecElementWithAttributes : public Identifiable {
 private:
  std::map<std::string, std::shared_ptr<AttributeValue>> m_attribute_values; // key = identifier from definition

 public:
  auto& attribute_values() { return m_attribute_values; }
};

class SpecType : public Identifiable {
 private:
  std::map<std::string, std::shared_ptr<AttributeDefinition>> m_attribute_definitions;
  std::map<std::string, std::shared_ptr<AttributeValueFactoryBase>> m_value_factories;

  void AddCommonAttribute(std::shared_ptr<AttributeDefinition> def, std::shared_ptr<AttributeValueFactoryBase> f) {
      m_attribute_definitions[def->identifier] = def;
      m_value_factories[def->identifier] = f;
  }

 public:
  void AddAttribute(std::shared_ptr<AttributeDefinitionString> def, std::shared_ptr<AttributeValueFactory<AttributeValueString>> f) {
    AddCommonAttribute(def, f);
  }

  void AddAttribute(std::shared_ptr<AttributeDefinitionEnumeration> def, std::shared_ptr<AttributeValueFactory<AttributeValueEnumeration>> f) {
    AddCommonAttribute(def, f);
  }

  void AddAttribute(std::shared_ptr<AttributeDefinitionInteger> def, std::shared_ptr<AttributeValueFactory<AttributeValueInteger>> f) {
    AddCommonAttribute(def, f);
  }

  auto& value_factories() { return m_value_factories; }
};

class SpecObjectType : public SpecType {
};

class SpecificationType : public SpecType {
};

class SpecObject : public SpecElementWithAttributes {
 private:
  std::shared_ptr<SpecType> type;

 public:
  SpecObject(std::shared_ptr<SpecType> _type) : type(_type) {}
};

class SpecHierachy : public AccessControlledElement {
 public:
  bool is_table_tnternal;
  std::list<SpecHierachy> children;
  std::shared_ptr<SpecificationType> type;
  std::map<std::string, std::shared_ptr<AttributeDefinition>> editable_attributes;
  std::shared_ptr<SpecObject> object;
};

class Specification : public SpecElementWithAttributes {
 public:
  std::list<SpecHierachy> children;
  std::shared_ptr<SpecificationType> type;
};

class Model {
 public:
  std::map<std::string, std::shared_ptr<DatatypeDefinition>> datatypes;
  std::map<std::string, std::shared_ptr<SpecObject>> objects;
  std::map<std::string, std::shared_ptr<SpecType>> spec_types;

  std::map<std::string, Specification> specifications;

  std::unique_ptr<SpecObject> create_object(std::shared_ptr<SpecObjectType> spec_type) {
    auto object = std::make_unique<SpecObject>(spec_type);

    for(auto& f : spec_type->value_factories()) {
      auto& values = object->attribute_values();
      values[f.first] = f.second->create_value();
    }

    return object;
  }
};

// TODO: AttributeValueEnumeration + AttributeValueEnumerationFactory