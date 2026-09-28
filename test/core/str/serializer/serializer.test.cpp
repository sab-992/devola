#include <gtest/gtest.h>

#include <core/str/serializer_factory.hpp>
#include <helper/core/inner_types.hpp>


template<typename T>
class SerializerTest : public ::testing::Test {
protected:
    auto getTestObject(bool alt=false) {
        return InnerTypes<T>::getTestObject(InnerTypes<T>::getTestString(alt));
    }

    std::string getTestStringObject(bool alt=false) {
        return InnerTypes<T>::getTestStringFromObject(this->getTestObject(alt));
    }
};

TYPED_TEST_SUITE(SerializerTest, innerTypes_t);

TYPED_TEST(SerializerTest, Serialize_ConvertsObjectIntoValidString) {
    auto serializer = serializer_n::Factory<TypeParam>::create();
    const auto EXPECTED_OBJECT = this->getTestObject();

    std::string serializedObject = serializer->serialize(EXPECTED_OBJECT);

    EXPECT_EQ(EXPECTED_OBJECT, InnerTypes<TypeParam>::getTestObject(serializedObject));
}

TYPED_TEST(SerializerTest, Deserialize_ConvertsStringIntoValidObject) {
    auto serializer = serializer_n::Factory<TypeParam>::create();
    const std::string EXPECTED_STRING_OBJECT = this->getTestStringObject();

    const auto serializedObject = serializer->deserialize(EXPECTED_STRING_OBJECT);

    EXPECT_EQ(InnerTypes<TypeParam>::getTestObject(EXPECTED_STRING_OBJECT), serializedObject);
}