#include "model.hpp"
#include <iostream>
//#include "gtest/gtest.h"

//TEST(TestModel, CreateSpecification) {
int main() {
  Model m;

  // Datatype
  auto dts = std::make_shared<DatatypeDefinitionString>();
  m.datatypes[dts->identifier] = dts;
  dts->max_length = 3;

  // AttributeDefinition
  auto ads = std::make_shared<AttributeDefinitionString>(dts);

  // DefaultValue
  AttributeValueString default_value{ads};
  default_value.set_value("v1");

  // SpecObjectType
  auto sot = std::make_shared<SpecObjectType>();
  auto f = std::make_shared<AttributeValueStringFactory>(default_value);
  sot->AddAttribute(ads, f);
  m.spec_types[sot->identifier] = sot;

  //create a requirement
  std::shared_ptr<SpecObject> so1 = m.create_object(sot);
  m.objects[so1->identifier] = so1;

  auto& values1 = so1->attribute_values();
  auto& value1 = values1[ads->identifier];
  std::cout << value1 ->value() << std::endl;
  std::cout << value1->set_value("123") << std::endl;
  std::cout << value1->value() << std::endl;
  std::cout << value1->set_value("1234") << std::endl;
  std::cout << value1->value() << std::endl;
  std::cout << f->default_value().value() << std::endl;

  //create another requirement
  f->default_value().set_value("v2");
  std::shared_ptr<SpecObject> so2 = m.create_object(sot);
  //so2->identifier = "so2";
  m.objects[so2->identifier] = so2;

  auto& values2 = so2->attribute_values();
  auto& value2 = values2[ads->identifier];
  std::cout << value2->value() << std::endl;

  return 0;
}
