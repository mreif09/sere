#include <string>
#include <memory>
#include <list>

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
  bool is_editable;
};

class DatatypeDefinition: public Identifiable {
};

class DatatypeDefinitionSimple: public DatatypeDefinition {
};

class DatatypeDefinitionString: public DatatypeDefinitionSimple{
  int max_length;
};

class AttributeDefinition : public AccessControlledElement {
  virtual bool multi_valued() const = 0;
  virtual bool is_value_valid(const std::string&) const = 0;
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
  bool add_value(const std::string&) override { return false; }
  void remove_value(const std::string&) override {}
  void clear_values() override {}
};

class AttributeValueString : public AttributeValueSimple {
 private:
  std::string m_value;

 public:
  std::shared_ptr<DatatypeDefinitionString> type;
  std::shared_ptr<AttributeDefinition> definition;

  bool set_value(const std::string& value) override {
    m_value = value;
    return true;
  }

  std::string value() const override { return m_value; }
};

class AttributeDefinitionSimple : public AttributeDefinition {
 public:
  bool multi_valued() const override { return false; };
};

class AttributeDefinitionString : public AttributeDefinitionSimple {
 public:
  AttributeValueString default_value;
  std::shared_ptr<DatatypeDefinitionString> type;

  virtual bool is_value_valid(const std::string& value) {
    return false;
  }

  std::unique_ptr<AttributeValueString> create_value() {
    return std::make_unique<AttributeValueString>(default_value);
  }
};

class SpecElementWithAttributes : public Identifiable {
 public:
  std::list<std::unique_ptr<AttributeValue>> attribute_values;
};

class SpecType : public Identifiable {
 public:
  std::list<std::shared_ptr<AttributeDefinition>> attributes_definitions;
};

class SpecObjectType : public SpecType {
};

class SpecObject : public SpecElementWithAttributes {
  std::shared_ptr<SpecObjectType> type;
};

class SpecificationType : public SpecType {
};

class SpecHierachy : public AccessControlledElement {
  bool is_table_tnternal;
  std::list<SpecHierachy> children;
  std::shared_ptr<SpecificationType> type;
  std::list<std::shared_ptr<AttributeDefinition>> editable_attributes;
  std::shared_ptr<SpecObject> object;
};

class Specification : public SpecElementWithAttributes {
  std::list<SpecHierachy> children;
  std::shared_ptr<SpecificationType> type;
};

class Model {
  std::list<std::shared_ptr<DatatypeDefinition>> datatypes;
  std::list<std::shared_ptr<Specification>> specificatons;
  std::list<std::shared_ptr<SpecObject>> objects;
  std::list<std::shared_ptr<SpecType>> spec_types;
};
