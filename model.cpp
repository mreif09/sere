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
};

class AttributeDefinitionSimple : public AttributeDefinition {
};

class AttributeDefinitionString : public AttributeDefinitionSimple {
  std::string default_value;
  std::shared_ptr<DatatypeDefinitionString> type;
};

class AttributeValue {
};

class AttributeValueSimple {
};

class AttributeValueString : public AttributeValueSimple {
  std::string value;
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
