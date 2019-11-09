#include "model.hpp"
#include <iostream>
//#include "gtest/gtest.h"

//TEST(TestModel, CreateSpecification) {
int main() {
  Model m;

  auto dts = std::make_shared<DatatypeDefinitionString>();
  dts->identifier = "dt1";
  m.datatypes[dts->identifier] = dts;
  dts->max_length = 3;

//  m.specifications["spec1"] = Specification{};
//  auto& spec = m.specifications["spec1"];
//  spec.identifier = "spec1";

//  auto ads = std::make_shared<AttributeDefinitionString>(dts);
  auto ads = std::make_shared<AttributeDefinitionString>(dts);
  ads->identifier = "ad1";

  AttributeValueString default_value{ads};
//  default_value.
  default_value.set_value("v1");

  auto f = std::make_shared<AttributeValueStringFactory>(default_value);

  auto sot = std::make_shared<SpecObjectType>();
  sot->identifier = "st1";
  //sot->attribute_definitions[ads->identifier] = ads;
  sot->AddAttribute(ads, f);
  m.spec_types[sot->identifier] = sot;
 
  //auto so = std::make_shared<SpecObject>(sot);
  std::shared_ptr<SpecObject> so = m.create_object(sot);
  so->identifier = "so1";
  m.objects[so->identifier] = so;

  //so->attribute_values[ads->identifier] = ads->create_value();
  //ads->create_value();
  //so->attribute_values[ads->identifier] = ads->create_value();

  auto& value = so->attribute_values[ads->identifier];
  std::cout << value->value() << std::endl;
  std::cout << value->set_value("123") << std::endl;
  std::cout << value->value() << std::endl;
  std::cout << value->set_value("1234") << std::endl;
  std::cout << value->value() << std::endl;
  std::cout << f->m_default_value.value() << std::endl;

  return 0;
}
