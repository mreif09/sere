#include <string>
#include <memory>
#include <list>
#include <map>

class Identifiable {
 public:
  std::string desc;
  std::string identifier;

  std::string long_name;

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
};

class DatatypeDefinitionSimple: public DatatypeDefinition {
};

class DatatypeDefinitionString: public DatatypeDefinitionSimple{
 public:
  int max_length;
};

class AttributeDefinition : public AccessControlledElement {
 public:
  virtual bool multi_valued() const = 0;
  virtual bool is_value_valid(const std::string&) const = 0;

  //virtual std::unique_ptr<AttributeValue> create_value() const = 0;
};

class AttributeDefinitionSimple : public AttributeDefinition {
 public:
  bool multi_valued() const override { return false; };
};

class AttributeDefinitionString : public AttributeDefinitionSimple {
 public:
  AttributeDefinitionString(std::shared_ptr<DatatypeDefinitionString> _type)
      : type(_type) {}

// public:
//  static std::shared_ptr<AttributeDefinitionString> create(std::shared_ptr<DatatypeDefinitionString> _type) {
//    auto ads = std::shared_ptr<AttributeDefinitionString>(new AttributeDefinitionString{_type});
//    ads->default_value.definition = ads;
//    ads->default_value.type = _type;
//    return ads;
//  }

  std::shared_ptr<DatatypeDefinitionString> type;

  bool is_value_valid(const std::string& value) const override {
    return value.length() <= type->max_length;
  }

//  std::unique_ptr<AttributeValue> create_value() const override {
//    return std::make_unique<AttributeValueString>(default_value);
//  }
};

class AttributeValue {
 public:
  virtual bool set_value(const std::string&) = 0;
  virtual std::string value() const = 0;
  virtual bool add_value(const std::string&) = 0;
  virtual void remove_value(const std::string&) = 0;
  virtual void clear_values() = 0;
};

class AttributeValueSimple : public AttributeValue {
 public:
  bool add_value(const std::string&) override { return false; }
  void remove_value(const std::string&) override {}
  void clear_values() override {}
};

class AttributeValueString : public AttributeValueSimple {
 private:
  std::string m_value;

 public:
  AttributeValueString(std::shared_ptr<AttributeDefinitionString> _definition)
	: definition{_definition} {}

  std::shared_ptr<AttributeDefinitionString> definition;

  bool set_value(const std::string& value) override {
    if (definition->is_value_valid(value) == false) return false;
    m_value = value;
    return true;
  }

  std::string value() const override { return m_value; }
};

class AttributeValueFactory {
 public:
  virtual std::unique_ptr<AttributeValue> create_value() const = 0;
};

class AttributeValueStringFactory : public AttributeValueFactory {
 public:
  AttributeValueString m_default_value;

    AttributeValueStringFactory(const AttributeValueString& default_value)
	: m_default_value{default_value} {}
	
  std::unique_ptr<AttributeValue> create_value() const override {
    return std::make_unique<AttributeValueString>(m_default_value);
  }
};


class SpecElementWithAttributes : public Identifiable {
 public:
  std::map<std::string, std::unique_ptr<AttributeValue>> attribute_values; // key = identifier from definition
};

class SpecType : public Identifiable {
 public:
  std::map<std::string, std::shared_ptr<AttributeDefinition>> attribute_definitions;
};

class SpecObjectType : public SpecType {
 public:
  std::map<std::string, std::shared_ptr<AttributeValueFactory>> value_factories;

  void AddAttribute(std::shared_ptr<AttributeDefinition> def, std::shared_ptr<AttributeValueFactory> f) {
      attribute_definitions[def->identifier] = def;
      value_factories[def->identifier] = f;
  }

};

class SpecificationType : public SpecType {
};

class SpecObject : public SpecElementWithAttributes {
 public:
  SpecObject(std::shared_ptr<SpecType> _type) : type(_type) {}

  std::shared_ptr<SpecType> type;
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

    for(auto& f : spec_type->value_factories) {
      object->attribute_values[f.first] = f.second->create_value();
    }

    return object;
  }
};
