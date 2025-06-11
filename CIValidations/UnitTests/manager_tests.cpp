#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include "Manager/YamlHelper.h"

TEST_CASE("OverrideConfig modifies YAML nodes", "[Yamlhelper]") {
  YAML::Node lineup = YAML::Load("{1B: Prince Fielder, 2B: Rickie Weeks, LF: Ryan Braun}");

  REQUIRE(lineup.size() == 3);
  REQUIRE(lineup["1B"].as<std::string>() == "Prince Fielder");

  OverrideConfig(lineup, "1B", "Fielder formerly know as Prince");
  REQUIRE(lineup["1B"].as<std::string>() == "Fielder formerly know as Prince");

  OverrideConfig(lineup, "1B", 123);
  REQUIRE(lineup["1B"].as<unsigned>() == 123u);
}

TEST_CASE("STRINGtoYAML parses YAML from string", "[Yamlhelper]") {
  std::string Text = "{1B: I like blarb, 2B: whatever, LF: mach3, 3B: Britannia}";
  YAML::Node TextNode = STRINGtoYAML(Text);

  REQUIRE(TextNode["1B"].as<std::string>() == "I like blarb");
  REQUIRE(TextNode["2B"].as<std::string>() == "whatever");
}

TEST_CASE("CheckNodeExists works as expected", "[Yamlhelper]") {
  std::string Text = "{1B: I like blarb, 2B: whatever, LF: mach3}";
  YAML::Node TextNode = STRINGtoYAML(Text);

  REQUIRE(CheckNodeExists(TextNode, "1B") == true);
  REQUIRE(CheckNodeExists(TextNode, "Tower", "Bridge") == false);
}

TEST_CASE("GetFromManager retrieves values with defaults", "[Yamlhelper]") {
  std::string Text = "{1B: I like blarb, 2B: whatever, LF: mach3}";
  YAML::Node TextNode = STRINGtoYAML(Text);

  REQUIRE(GetFromManager<std::string>(TextNode["1B"], "London") == "I like blarb");
  REQUIRE(GetFromManager<std::string>(TextNode["10B"], "London") == "London");
}


TEST_CASE("Test YAML merging", "[Yamlhelper]") {
  SECTION("Test merging with overlapping and distinct nested keys") {
    std::string yamlContent = R"(
Person:
  name: "bob bob"
  skill: "lagrangian time"

City:
  name: Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch
)";

    std::string yamlContent2 = R"(
Person:
  name: "grzegorz brzęczyszczykiewicz"
  age: "66"

Building:
  name: "cinema"
)";

    // Convert the string to a YAML node
    YAML::Node Config1 = STRINGtoYAML(yamlContent);
    YAML::Node Config2 = STRINGtoYAML(yamlContent2);

    YAML::Node ConfigMerged = MergeNodes(Config1, Config2);

    // Test that nested key "name" in "Person" is overwritten
    REQUIRE(ConfigMerged["Person"]["name"].as<std::string>() == "grzegorz brzęczyszczykiewicz");
    // Test that other existing nested keys in "Person" are preserved
    REQUIRE(ConfigMerged["Person"]["skill"].as<std::string>() == "lagrangian time");
    // Test that new nested keys in "Person" are added
    REQUIRE(ConfigMerged["Person"]["age"].as<int>() == 66);
    // Test that entirely new top-level keys are added
    REQUIRE(ConfigMerged["Building"]["name"].as<std::string>() == "cinema");
    // Test that keys from the first node not present in the second are preserved
    REQUIRE(ConfigMerged["City"]["name"].as<std::string>() == "Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch");

    ////// Test that map in `b` overwrites scalar in `a`
    std::string yamlA = R"(key: value)";
    std::string yamlB = R"(key: { nested: true })";

    YAML::Node A = STRINGtoYAML(yamlA);
    YAML::Node B = STRINGtoYAML(yamlB);
    YAML::Node Merged = MergeNodes(A, B);

    REQUIRE(Merged["key"]["nested"].as<bool>() == true);
  }

  SECTION("Test with non-scalar key (complex key handling)")
  {
    YAML::Node A;
    YAML::Node complexKey;
    complexKey["foo"] = "bar";
    A[complexKey] = "value";

    YAML::Node B;
    B[complexKey] = "new_value";

    YAML::Node Merged = MergeNodes(A, B);

    // Ensure merging doesn't crash with complex (non-scalar) keys
    REQUIRE(Merged.size() == 1);
  }
  SECTION("Test null in `b` does not overwrite value in `a`")
  {
    std::string yamlA = R"(key: value)";
    std::string yamlB = R"(key: ~)";  // YAML null

    YAML::Node A = STRINGtoYAML(yamlA);
    YAML::Node B = STRINGtoYAML(yamlB);
    YAML::Node Merged = MergeNodes(A, B);

    // Confirm that null does not overwrite existing value
    REQUIRE(Merged["key"].as<std::string>() == "value");
  }

  SECTION("Test deep recursive merging of nested maps")
  {
    std::string yamlA = R"(
outer:
  inner:
    a: 1
    b: 2
)";
    std::string yamlB = R"(
outer:
  inner:
    b: 999
    c: 3
)";

    YAML::Node A = STRINGtoYAML(yamlA);
    YAML::Node B = STRINGtoYAML(yamlB);
    YAML::Node Merged = MergeNodes(A, B);

    // Ensure keys in nested maps are merged recursively
    REQUIRE(Merged["outer"]["inner"]["a"].as<int>() == 1); // preserved
    REQUIRE(Merged["outer"]["inner"]["b"].as<int>() == 999); // overwritten
    REQUIRE(Merged["outer"]["inner"]["c"].as<int>() == 3); // added
  }

 SECTION("Test when `a` is null"){
    YAML::Node A;  // Null node
    YAML::Node B = STRINGtoYAML("hello: world");

    YAML::Node Merged = MergeNodes(A, B);

    // Ensure full replacement when `a` is null
    REQUIRE(Merged["hello"].as<std::string>() == "world");
  }
}


TEST_CASE("Check GetBounds", "[Yamlhelper]") {
  SECTION("Check Default")
  {
    std::string Bounds = "Bounds: [0, 4]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    auto TempBoundsVec = GetBounds(TextNode["Bounds"]);

    REQUIRE_THAT(TempBoundsVec[0], Catch::Matchers::WithinAbs(0, 1e-6));
    REQUIRE_THAT(TempBoundsVec[1], Catch::Matchers::WithinAbs(4, 1e-6));
  }
  SECTION("Check with upper empty bound")
  {
    std::string Bounds = "Bounds: [0, \"\"]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    auto TempBoundsVec = GetBounds(TextNode["Bounds"]);

    REQUIRE_THAT(TempBoundsVec[0], Catch::Matchers::WithinAbs(0, 1e-6));
    REQUIRE_THAT(TempBoundsVec[1], Catch::Matchers::WithinAbs(M3::KinematicUpBound, 1e-6));
  }
  SECTION("Check with bottom empty bound")
  {
    std::string Bounds = "Bounds: [\"\", 10]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    auto TempBoundsVec = GetBounds(TextNode["Bounds"]);

    REQUIRE_THAT(TempBoundsVec[0], Catch::Matchers::WithinAbs(M3::KinematicLowBound , 1e-6));
    REQUIRE_THAT(TempBoundsVec[1], Catch::Matchers::WithinAbs(10, 1e-6));
  }
  SECTION("Check with both bounds empty")
  {
    std::string Bounds = "Bounds: [\"\", \"\"]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    auto TempBoundsVec = GetBounds(TextNode["Bounds"]);

    REQUIRE_THAT(TempBoundsVec[0], Catch::Matchers::WithinAbs(M3::KinematicLowBound , 1e-6));
    REQUIRE_THAT(TempBoundsVec[1], Catch::Matchers::WithinAbs(M3::KinematicUpBound , 1e-6));
  }

  SECTION("Check error handling for single variable")
  {
    std::string Bounds = "Bounds: [0]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    REQUIRE_THROWS(GetBounds(TextNode["Bounds"]));
  }
  SECTION("Check error handling for multiple variable")
  {
    std::string Bounds = "Bounds: [0, 10, 100]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    REQUIRE_THROWS(GetBounds(TextNode["Bounds"]));
  }
  SECTION("Check error handling for unspecified values")
  {
    std::string Bounds = "Bounds: [0, \"blarb\"]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    REQUIRE_THROWS(GetBounds(TextNode["Bounds"]));
  }
  SECTION("Check error handling for invalid type")
  {
    std::string Bounds = "Bounds: [0, {val: 10}]";
    YAML::Node TextNode = STRINGtoYAML(Bounds);
    REQUIRE_THROWS(GetBounds(TextNode["Bounds"]));
  }
}

TEST_CASE("Check M3OpenConfig", "[Yamlhelper]") {

  SECTION("Invalid file extension throws") {
    REQUIRE_THROWS(M3OpenConfig("config.toml"));
  }

  SECTION("Nonexistent file throws") {
    REQUIRE_THROWS(M3OpenConfig("does_not_exist.yaml"));
  }

  SECTION("YAML file with bad dash formatting throws") {
    std::ofstream out("bad.yaml");
    out << "Config:\n"
    "-badValue: 1\n";  // no space after '-'
    out.close();

    REQUIRE_THROWS(M3OpenConfig("bad.yaml"));

    std::remove("bad.yaml");
  }

  SECTION("Valid YAML file loads successfully") {
    std::ofstream out("valid.yaml");
    out << "Config:\n"
    "- goodValue: 1\n";  // has space after '-'
    out.close();

    YAML::Node config;
    REQUIRE_NOTHROW(config = M3OpenConfig("valid.yaml"));
    REQUIRE(config["Config"]);
    REQUIRE(config["Config"][0]["goodValue"].as<int>() == 1);

    std::remove("valid.yaml");
  }

  SECTION("YAML with document separator '---' doesn't throw") {
    std::ofstream out("doc_sep.yaml");
    out << "---\nConfig:\n  Value: 42\n";
    out.close();

    YAML::Node config;
    REQUIRE_NOTHROW(config = M3OpenConfig("doc_sep.yaml"));
    REQUIRE(config["Config"]["Value"].as<int>() == 42);

    std::remove("doc_sep.yaml");
  }

  SECTION("YAML with multi-line flow-style list parses successfully") {
    std::ofstream out("multiline_flow.yaml");
    out << "YourScore: [-2000.0, -1960.0, -1920.0, -1880.0, -1840.0, -1800.0,\n"
    "            -1760.0, -1720.0, -1680.0, -1640.0, -1600.0, -1560.0]\n";
    out.close();

    YAML::Node config;
    REQUIRE_NOTHROW(config = M3OpenConfig("multiline_flow.yaml"));
    REQUIRE(config["YourScore"]);
    REQUIRE(config["YourScore"].IsSequence());
    REQUIRE(config["YourScore"].size() == 12);
    REQUIRE(config["YourScore"][0].as<double>() == -2000.0);
    REQUIRE(config["YourScore"][11].as<double>() == -1560.0);

    std::remove("multiline_flow.yaml");
  }

  SECTION("YAML with comments and indentation parses correctly") {
    std::ofstream out("comments.yaml");
    out << "# This is a comment\n"
    "Config:\n"
    "  - value1: 10  # inline comment\n"
    "  - value2: 20\n";
    out.close();

    YAML::Node config;
    REQUIRE_NOTHROW(config = M3OpenConfig("comments.yaml"));
    REQUIRE(config["Config"].IsSequence());
    REQUIRE(config["Config"][0]["value1"].as<int>() == 10);
    REQUIRE(config["Config"][1]["value2"].as<int>() == 20);

    std::remove("comments.yaml");
  }

  SECTION("YAML with nested flow-style lists parses successfully") {
    std::ofstream out("nested_flow.yaml");
    out << "Nested: [[1, 2], [3, 4], [5, 6]]\n";
    out.close();

    YAML::Node config;
    REQUIRE_NOTHROW(config = M3OpenConfig("nested_flow.yaml"));
    REQUIRE(config["Nested"].IsSequence());
    REQUIRE(config["Nested"][0].IsSequence());
    REQUIRE(config["Nested"][0][0].as<int>() == 1);
    REQUIRE(config["Nested"][2][1].as<int>() == 6);

    std::remove("nested_flow.yaml");
  }

  SECTION("YAML with multiple documents parses successfully") {
    std::ofstream out("multi_doc.yaml");
    out << "---\nConfig1:\n  Value: 1\n---\nConfig2:\n  Value: 2\n";
    out.close();

    YAML::Node config;
    REQUIRE_NOTHROW(config = M3OpenConfig("multi_doc.yaml"));
    REQUIRE(config["Config1"]["Value"].as<int>() == 1);

    std::remove("multi_doc.yaml");
  }

  SECTION("YAML with unicode characters parses successfully") {
    std::ofstream out("unicode.yaml");
    out << "Greeting: \"こんにちは\"\n";
    out.close();

    YAML::Node config;
    REQUIRE_NOTHROW(config = M3OpenConfig("unicode.yaml"));
    REQUIRE(config["Greeting"].as<std::string>() == "こんにちは");

    std::remove("unicode.yaml");
  }
}
