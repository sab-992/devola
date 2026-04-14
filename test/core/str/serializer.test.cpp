#include <gtest/gtest.h>

#include <core/str/detail/serializer/json.h>
#include <core/str/detail/serializer/string.h>
#include <core/str/detail/serializer/xml.h>
#include <utils/core/inner_types.h>


using serializer_t = ::testing::Types<serializer_n::JSON, serializer_n::XML, serializer_n::String>;

template<typename Serializer>
class SerializerTest : public ::testing::Test {
    template<typename ContentType>
    struct TemplatedSerializer {
        using type = serializer_n::Serializer_i<ContentType>;
    };

public:
    using Inner = InnerTypes<Serializer, SerializerTest<Serializer>::template TemplatedSerializer>;

    auto getTestObject(bool alt=false) {
        return Inner::getTestObject(Inner::getTestStringObject(alt));
    }

    std::string getTestStringObject(bool alt=false) {
        return Inner::getTestStringObject(alt);
    }
};

TYPED_TEST_SUITE_P(SerializerTest);

TYPED_TEST_P(SerializerTest, Serialize_ConvertsObjectIntoValidString) {
    TypeParam serializer;
    const auto EXPECTED_OBJECT = this->getTestObject();

    std::string serializedObject = serializer.serialize(EXPECTED_OBJECT);

    EXPECT_EQ(EXPECTED_OBJECT, SerializerTest<TypeParam>::Inner::getTestObject(serializedObject));
}

TYPED_TEST_P(SerializerTest, Deserialize_ConvertsStringIntoValidObject) {
    TypeParam serializer;
    const std::string EXPECTED_STRING_OBJECT = this->getTestStringObject();

    const auto serializedObject = serializer.deserialize(EXPECTED_STRING_OBJECT);

    EXPECT_EQ(SerializerTest<TypeParam>::Inner::getTestObject(EXPECTED_STRING_OBJECT), serializedObject);
}

REGISTER_TYPED_TEST_SUITE_P(SerializerTest, Serialize_ConvertsObjectIntoValidString,
                                            Deserialize_ConvertsStringIntoValidObject);

INSTANTIATE_TYPED_TEST_SUITE_P(SerializerTestSuite, SerializerTest, serializer_t);