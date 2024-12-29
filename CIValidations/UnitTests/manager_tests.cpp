#include "catch2/catch_test_macros.hpp"

#include "manager/YamlHelper.h"

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
