// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_template.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <sstream>

#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/markdown/markdown_generator.hpp"

#include "inja_formatter.impl.hpp"

namespace standardese::formatter {

namespace {

template <typename T>
std::string to_string(const T* ptr) {
  std::ostringstream adr;
  adr << static_cast<const void*>(ptr);
  return adr.str();
}

template <typename T>
const T* from_string(const nlohmann::json::string_t* value) {
  std::istringstream adr(*value);
  std::uintptr_t parsed;
  adr >> std::hex >> parsed;
  return reinterpret_cast<const T*>(parsed);
}

}

nlohmann::json inja_formatter::to_json(const model::entity& entity) const {
  nlohmann::json json;

  model::visitor::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_same_v<T, model::module>) {
      json["standardese"] = {
        {"kind", "module"},
        {"name", entity.name },
      };
    } else if constexpr (std::is_same_v<T, model::document>) {
      json["standardese"] = {
        {"kind", "document"},
        {"name", entity.name},
        {"path", entity.path},
      };
    } else if constexpr (std::is_same_v<T, model::group_documentation>) {
      json["standardese"] = {
        {"kind", "group"},
        {"entities", nlohmann::json::array()},
      };
      for (auto& member : entity.entities)
        json["standardese"]["entities"].push_back(to_json(member.entity()));
    } else if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      // TODO: Are we losing \synopsis here?
      json.merge_patch(to_json(entity.entity()));
    }

    if constexpr (std::is_base_of_v<model::mixin::documentation, T>) {
      json["standardese"]["group"] = entity.group.has_value() ? entity.group.value() : "";
      json["standardese"]["output_section"] = entity.output_section.has_value() ? entity.output_section.value() : "";
      json["standardese"]["synopsis"] = entity.synopsis.has_value() ? entity.synopsis.value() : "";
    }

    struct serialization_generator : output_generator::markdown::markdown_generator {
      serialization_generator(std::ostream& os, const inja_formatter& self) : markdown_generator(os), self(self) {}

      void visit(link& link) override {
        auto serializable = link;

        serializable.target.accept([&](auto&& target) -> void {
          using T = std::decay_t<decltype(target)>;
          if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
            serializable.target = model::link_target::uri_target(self.target(*target.target));
          }
        });

        markdown_generator::visit(serializable);
      }

      const inja_formatter& self;
    };

    {
      std::stringstream stream;
      {
        auto generator = serialization_generator(stream, *this);
        entity.accept(generator);
      }
      json["md"] = stream.str();
    }
  }, entity);

  return json;
}

nlohmann::json inja_formatter::to_json(const cppast::cpp_entity& entity) const {
  nlohmann::json json;

  json["standardese"] = {
    {"kind", "cpp_entity"},
    {"value", to_string(&entity) },
  };

  // TODO: Expose all this as callbacks.
  /*
  if (entity.kind() == cppast::cpp_file::kind()) {
    json["name"] = boost::filesystem::path(entity.name()).filename().native();
  } else {
    json["name"] = entity.name();
  }

  switch(entity.kind()) {
    case cppast::cpp_entity_kind::file_t:
      json["cppast_kind"] = "file";
      json["kind"] = "File";
      break;
    case cppast::cpp_entity_kind::member_function_t:
      json["cppast_kind"] = "member_function";
      json["kind"] = "Member Function";
      break;
    case cppast::cpp_entity_kind::constructor_t:
      json["cppast_kind"] = "constructor";
      json["kind"] = "Constructor";
      break;
    case cppast::cpp_entity_kind::function_t:
      {
        const auto& function = static_cast<const cppast::cpp_function&>(entity);

        // TODO: Distinguish static member functions here?
        json["cppast_kind"] = "function";
        json["kind"] = "Function";
        // TODO
        json["declaration_specifiers"] = std::vector<std::string>{};
        // TODO
        json["const_qualification"] = "";
        // TODO
        json["volatile_qualification"] = "";
        // TODO
        json["ref_qualification"] = "";
        // TODO
        json["noexcept_specification"] = nullptr;

        std::vector<nlohmann::json> parameters;
        for (const auto& param : function.parameters())
          parameters.push_back(to_json(param));
        json["parameters"] = std::move(parameters);
      }
      break;
    case cppast::cpp_entity_kind::class_t:
      json["cppast_kind"] = "class";
      // TODO: Distinguish struct here?
      json["kind"] = "Class";
      break;
    case cppast::cpp_entity_kind::function_parameter_t:
      json["cppast_kind"] = "function_parameter";
      json["kind"] = "Parameter";
      break;
    default:
      // TODO: Implement me;
      json["cppast_kind"] = "not-implemented";
      json["kind"] = "not-implemented";
      break;
  }

  {
    const auto* parent = &entity;
    while(parent->parent().has_value())
      parent = &parent->parent().value();
    json["path"] = static_cast<const cppast::cpp_file&>(*parent).name();
  }

  // self->env.add_callback("name", [&entity](inja::Arguments& args) {
  //   name_modifiers modifiers;
  //   for (const auto& arg : args) {
  //     const std::string& modifier = arg->get<std::string>();
  //     if (modifier == "short") {
  //       modifiers = name_modifiers::shoort;
  //     } else {
  //       logger::warn(fmt::format("Ignoring unknown flag to `name` template `{}`.", modifier));
  //     }
  //   }

  //   return render_name(entity, modifiers);
  // });

  // TODO: Do we need this?
  // self->env.add_callback("link", 1, [this, &entity](inja::Arguments& args) {
  //   // TODO: Do not create the link if it is trivial, i.e., going back to the context.
  //   auto link = model::markup::link(entity, "");
  //   
  //   impl::foreach([&](const model::entity& child) {
  //     link.add_child(child);
  //   }, self->parse(args.at(0)->get<std::string>()));

  //   return self->render(link);
  // });
  */

  return json;
}

nlohmann::json inja_formatter::to_json(const cppast::cpp_type& type) const {
  nlohmann::json json = {
    {"standardese", {
      {"kind", "cpp_type"},
      {"value", to_string(&type) }
    }
  }};

  switch(type.kind()) {
    case cppast::cpp_type_kind::array_t:
      json["standardese"]["cppast_kind"] = "array";
      break;
    case cppast::cpp_type_kind::auto_t:
      json["standardese"]["cppast_kind"] = "auto";
      break;
    case cppast::cpp_type_kind::builtin_t:
      json["standardese"]["cppast_kind"] = "builtin";
      // TODO
      // const auto& builtin_type = static_cast<const cppast::cpp_builtin_type&>(type);
      // json["name"] = cppast::to_string(builtin_type.builtin_type_kind());
      // json["target"] = std::string{};
      break;
    case cppast::cpp_type_kind::cv_qualified_t:
      json["standardese"]["cppast_kind"] = "cv-qualified";
      // TODO
      // json["target"] = std::string{};
      // const auto& cv_qualified_type = static_cast<const cppast::cpp_cv_qualified_type&>(type);
      // env.add_callback("cv-qualifier", [&]() {
      //   switch(cv_qualified_type.cv_qualifier()) {
      //     case cppast::cpp_cv_const:
      //       return "const";
      //     case cppast::cpp_cv_const_volatile:
      //       return "const volatile";
      //     case cppast::cpp_cv_volatile:
      //       return "volatile";
      //     case cppast::cpp_cv_none:
      //     default:
      //       // TODO: Log assertion.
      //       return "";
      //   }
      // });
      // env.add_callback("name", [&]() {
      //   switch(cv_qualified_type.cv_qualifier()) {
      //     case cppast::cpp_cv_const:
      //       return build("const {{ name }}", cv_qualified_type.type(), context);
      //     case cppast::cpp_cv_const_volatile:
      //       return build("const volatile {{ name }}", cv_qualified_type.type(), context);
      //     case cppast::cpp_cv_volatile:
      //       return build("volatile {{ name }}", cv_qualified_type.type(), context);
      //     case cppast::cpp_cv_none:
      //     default:
      //       // TODO: Log assertion.
      //       return build("{{ name }}", cv_qualified_type.type(), context);
      //   }
      // });
      // env.add_callback("type", [&]() {
      //   return build(cv_qualified_type.type(), context);
      // });
      break;
    case cppast::cpp_type_kind::decltype_auto_t:
      json["standardese"]["cppast_kind"] = "decltype-auto";
      break;
    case cppast::cpp_type_kind::decltype_t:
      json["standardese"]["cppast_kind"] = "decltype";
      break;
    case cppast::cpp_type_kind::dependent_t:
      json["standardese"]["cppast_kind"] = "dependent";
      break;
    case cppast::cpp_type_kind::function_t:
      json["standardese"]["cppast_kind"] ="function";
      break;
    case cppast::cpp_type_kind::member_function_t:
      json["standardese"]["cppast_kind"] = "member-function";
      break;
    case cppast::cpp_type_kind::member_object_t:
      json["standardese"]["cppast_kind"] = "member-object";
    case cppast::cpp_type_kind::pointer_t:
      json["standardese"]["cppast_kind"] = "pointer";
      // TODO
      // json["target"] = std::string{};
      // env.add_callback("name", [&]() {
      //   return build("{{ name }}*", pointer_type.pointee(), context);
      // });
      // env.add_callback("pointee", [&]() {
      //   return build(pointer_type.pointee(), context);
      // });
      break;
    case cppast::cpp_type_kind::reference_t:
      json["standardese"]["cppast_kind"] = "reference";
      // TODO
      // const auto& reference_type = static_cast<const cppast::cpp_reference_type&>(type);
      // json["target"] = std::string{};
      // env.add_callback("reference_kind", [&]() {
      //   switch (reference_type.reference_kind()) {
      //     case cppast::cpp_ref_lvalue:
      //       return "lvalue";
      //     case cppast::cpp_ref_rvalue:
      //       return "rvalue";
      //     case cppast::cpp_ref_none:
      //     default:
      //       // TODO: Log assertion.
      //       return "";
      //   }
      // });
      // env.add_callback("name", [&]() {
      //   switch (reference_type.reference_kind()) {
      //     case cppast::cpp_ref_lvalue:
      //       return build("{{ name }}&", reference_type.referee(), context);
      //     case cppast::cpp_ref_rvalue:
      //       return build("{{ name }}&&", reference_type.referee(), context);
      //     case cppast::cpp_ref_none:
      //     default:
      //       // TODO: Log assertion.
      //       return build("{{ name }}", reference_type.referee(), context);
      //   }
      // });
      // env.add_callback("referee", [&]() {
      //   return build(reference_type.referee(), context);
      // });
      break;
    case cppast::cpp_type_kind::template_instantiation_t:
      json["standardese"]["cppast_kind"] = "template-instantiation";
      // TODO
      // const auto& template_instantiation_type = static_cast<const cppast::cpp_template_instantiation_type&>(type);

      // // cppast does provide the primary template here, i.e., the `std::vector`
      // // in `std::vector<int>`. Unfortunately, when this template is not
      // // defined in the parsed code, i.e., only pulled in from a header file,
      // // we do not get any information but the name of that type. E.g., in the
      // // `std::vector` case, we do not know that `std` is a namespace (it could
      // // in principle be another class.)
      // auto primary = template_instantiation_type.primary_template();

      // // The definition of the primary template if it is in our source code.
      // auto definition = self->context.map([&](auto&& context) -> type_safe::optional_ref<const cppast::cpp_entity> {
      //   const auto& definitions = primary.get(context.context().index());
      //   if (definitions.size() > 1) {
      //     // TODO: Complain. Only function overloads should have more than one definition.
      //   }
      //   if (definitions.size() >= 1) {
      //     return *definitions.begin();
      //   }
      //   return type_safe::nullopt;
      // });

      // if (definition.has_value()) {
      //   // This target is typically ignored since recursive rendering of the type does a better job here.
      //   json["target"] = self->embed(model::link_target(definition.value()));
      // } else {
      //   // TODO: Search the inventories for this definition.
      //   json["target"] = "";
      // }
      // json["name"] = template_instantiation_type.primary_template().name();
      // 
      break;
    case cppast::cpp_type_kind::template_parameter_t:
      json["standardese"]["cppast_kind"] = "template-parameter";
      break;
    case cppast::cpp_type_kind::unexposed_t:
      json["standardese"]["cppast_kind"] = "unexposed";
      break;
    case cppast::cpp_type_kind::user_defined_t:
      json["standardese"]["cppast_kind"] = "user-defined";
      break;
      // TODO
      // const auto& user_defined_type = static_cast<const cppast::cpp_user_defined_type&>(type);
      // json["name"] = user_defined_type.entity().name();

      // auto definition = self->context.map([&](auto&& context) -> type_safe::optional_ref<const cppast::cpp_entity> {
      //   const auto& definitions = user_defined_type.entity().get(context.context().index());
      //   if (definitions.size() > 1) {
      //     // TODO: Complain
      //   }
      //   if (definitions.size() >= 1) {
      //     return *definitions.begin();
      //   }
      //   return type_safe::nullopt;
      // });

      // if (!definition.has_value()) {
      //   // // This entity is not defined in the source code we parsed but comes
      //   // // from some other project's header file that was included.
      //   // // Unfortunately, cppast does not expose much information about such
      //   // // types yet.
      //   // env.add_callback("name", [&]() {
      //   //   std::string name = user_defined_type.entity().name();

      //   //   // TODO: Make this overridable with an optional parameter.
      //   //   // TODO: Think of some scheme so that the primary, e.g., the
      //   //   // function declarator can have a different option than things such
      //   //   // as parameter types.
      //   //   switch(options.namespace_display_options) {
      //   //     case code_formatter_options::namespace_display_options::full:
      //   //       // Nothing to do, name() is already fully qualified.
      //   //       break;
      //   //     case code_formatter_options::namespace_display_options::relative:
      //   //     {
      //   //       // Construct the namespace sequence containing `context`.
      //   //       std::stack<const cppast::cpp_namespace*> containing_namespaces; 

      //   //       const cppast::cpp_entity* walk = &context.entity();
      //   //       while (walk->parent().has_value()) {
      //   //         walk = &walk->parent().value();
      //   //         if (walk->kind() == cppast::cpp_namespace::kind())
      //   //           containing_namespaces.push(static_cast<const cppast::cpp_namespace*>(walk));
      //   //       }

      //   //       // Drop the shared prefix of this entity and `context`.
      //   //       while (!containing_namespaces.empty()) {
      //   //         const std::string prefix = containing_namespaces.top()->name() + "::";

      //   //         if (boost::algorithm::starts_with(name, prefix))
      //   //           name = name.substr(prefix.size());

      //   //         containing_namespaces.pop();
      //   //       }

      //   //       break;
      //   //     }
      //   //     case code_formatter_options::namespace_display_options::hidden:
      //   //       // cppast does not tell us in which namespace this entity is
      //   //       // defined. However, we would have to figure out which of the
      //   //       // leading `name::` bits are namespaces and which are not
      //   //       // (because they are e.g. classes.) We can also not ask cppast,
      //   //       // which namespaces exist so there seems to be no safe way to
      //   //       // find out currently; apart from that, that would not be correct
      //   //       // in general: A namespace in one translation unit can be
      //   //       // something else in another translation unit.
      //   //       break;
      //   //   }

      //   //   return name;
      //   // });

      //   // TODO: Can we do better here? For template types, cppast does
      //   // probably not provide enough information but for non-template types
      //   // we could maybe.
      //   json["target"] = std::string{};
      // } else {
      //   // env.add_callback("target", [&]() -> model::link_target {
      //   //   return model::link_target(definition);
      //   // });
      // }
  }

  return json;
}

// TODO
// nlohmann::json inja_formatter::to_json(const model::cpp_entity_documentation& data) {
//   nlohmann::json json = to_json(data.entity());
// 
//   self->env.add_callback("synopsis", [this, &data](inja::Arguments& args) {
//     // TODO: Do not ignore args.
//     return self->embed(synopsis_formatter({}).build(data));
//   });
// 
//   return json;
// }
// 
// nlohmann::json inja_formatter::to_json(model::cpp_entity_documentation& data) {
//   this->add_data(static_cast<const model::cpp_entity_documentation&>(data));
// 
//   const auto& entity = data.entity();
// 
//   // TODO: Test the different versions of this.
//   self->env.add_callback("brief", 1, [this, &data](inja::Arguments& args) {
//     const std::string policy = args.at(0)->get<std::string>();
// 
//     for (auto section = data.begin(); section != data.end(); ++section) {
//       if (section->is<model::section>() && section->as<model::section>().type == parser::commands::section_command::brief) {
//         model::document brief {"", ""};
//         for (const auto& block : section->as<model::section>())
//           brief.add_child(block);
// 
//         if (policy == "consume")
//           data.erase(section);
//         else
//           throw std::logic_error("not implemented: brief policy"); // TODO
// 
//         return self->embed(std::move(brief));
//       }
//     }
// 
//     return std::string{}; 
//   });
// }

nlohmann::json inja_formatter::to_json(const model::link_target& target) const {
  nlohmann::json json = {
    {"standardese", {
      { "kind", "link_target" },
      { "target", nlohmann::json{} }
    }}};

  target.accept([&](auto&& link) {
    using T = std::decay_t<decltype(link)>;

    if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
      json["standardese"]["target"] = to_json(*link.target);
    } else {
      // TODO
      logger::error("not implemented: to_json(link_target)");
    }
    return nlohmann::json{};
  });

  return json;
}

inja_formatter::impl::variant inja_formatter::impl::from_json(const nlohmann::json& value) {
  if (value.is_null())
    return nullptr;
  if (value.is_object()) {
    const auto standardese = value.find("standardese");
    if (standardese != value.end() && standardese->is_object()) {
      const auto kind = standardese->find("kind");
      if (kind != standardese->end() && kind->is_string()) {
        const auto& kind_ref = kind->get_ref<const nlohmann::json::string_t&>();
        if (kind_ref == "cpp_entity") {
          const auto value = standardese->find("value");
          if (value != standardese->end() && value->is_string())
            return from_string<cppast::cpp_entity>(value->get_ptr<const nlohmann::json::string_t*>());
        } else if (kind_ref == "module") {
          const auto value = standardese->find("name");
          if (value != standardese->end() && value->is_string())
            return model::module(value->get<std::string>());
        } else if (kind_ref == "link_target") {
          const auto target = standardese->find("target");
          if (target != standardese->end() && target->is_object()) {
            return std::visit([&](auto&& value) -> model::link_target {
              using T = std::decay_t<decltype(value)>;
              if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
                return model::link_target(*value);
              } else {
                // TODO
                throw std::logic_error(fmt::format("Could not restore link target from {}", nlohmann::to_string(*target)));
              }
            }, from_json(*target));
          }
        } else if (kind_ref == "cpp_type") {
          const auto value = standardese->find("value");
          if (value != standardese->end() && value->is_string())
            return from_string<cppast::cpp_type>(value->get_ptr<const nlohmann::json::string_t*>());
        }
      }
    }
    return value.get_ptr<const nlohmann::json::object_t*>();
  } else if (value.is_string()) {
    return value.get_ptr<const nlohmann::json::string_t*>();
  } else if (value.is_array()) {
    return value.get_ptr<const nlohmann::json::array_t*>();
  }

  // TODO
  throw std::logic_error(fmt::format("not implemented: from_json({})", nlohmann::to_string(value)));
}

}
