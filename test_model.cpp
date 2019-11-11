#include "model.hpp"
#include <iostream>
//#include "gtest/gtest.h"

//TEST(TestModel, CreateSpecification) {
int main() {
  Model m;

  // Datatypes
  auto dts = std::make_shared<DatatypeDefinitionString>();
  m.datatypes[dts->identifier] = dts;
  dts->max_length = 3;

  auto dti = std::make_shared<DatatypeDefinitionInteger>();
  m.datatypes[dts->identifier] = dti;
  dti->min = 1;
  dti->max = 3;

  auto dte = std::make_shared<DatatypeDefinitionEnumeration>();
  m.datatypes[dte->identifier] = dte;

  auto& enum_values = dte->enum_values();
  auto ev1 = std::make_shared<EnumValue>(0, "V0");
  enum_values[ev1->identifier] = ev1;

  auto ev2 = std::make_shared<EnumValue>(1, "V1");
  enum_values[ev2->identifier] = ev2;

  // AttributeDefinition
  auto ads = std::make_shared<AttributeDefinitionString>(dts);
  auto ade = std::make_shared<AttributeDefinitionEnumeration>(dte);
  auto adi = std::make_shared<AttributeDefinitionInteger>(dti);

  // DefaultValue
  AttributeValueString dvs{ads};
  dvs.set_value("v1");

  AttributeValueEnumeration dve{ade};
  dve.add_value("V0");
  dve.add_value("V1");
  dve.add_value("---");

  AttributeValueInteger dvi{adi};
  dvi.set_value("1");

  // SpecObjectType
  auto sot = std::make_shared<SpecObjectType>();

  auto f1 = std::make_shared<AttributeValueFactory<AttributeValueString>>(dvs);
  sot->AddAttribute(ads, f1);

  auto f2 = std::make_shared<AttributeValueFactory<AttributeValueEnumeration>>(dve);
  sot->AddAttribute(ade, f2);
  m.spec_types[sot->identifier] = sot;

  auto f3 = std::make_shared<AttributeValueFactory<AttributeValueInteger>>(dvi);
  sot->AddAttribute(adi, f3);

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
  std::cout << f1->default_value().value() << std::endl;

  auto& valuee1 = values1[ade->identifier];
  for(auto& value : valuee1->values()) {
    std::cout << value << ",";
  }
  std::cout << std::endl;

  auto& valuei1 = values1[adi->identifier];
  std::cout << valuei1->value() << std::endl;

  //create another requirement
  f1->default_value().set_value("v2");
  std::shared_ptr<SpecObject> so2 = m.create_object(sot);
  //so2->identifier = "so2";
  m.objects[so2->identifier] = so2;

  auto& values2 = so2->attribute_values();
  auto& value2 = values2[ads->identifier];
  std::cout << value2->value() << std::endl;

  return 0;
}
