// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "../../standardese/transformer/create_link_text_transformer.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/link_target.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

template<class> inline constexpr bool always_false_v = false;

namespace standardese::transformer {

create_link_text_transformer::create_link_text_transformer(model::unordered_entities& documents, parser::cpp_context context, link_text_transformer_options options) : transformer(documents), options(std::move(options)), context(std::move(context)) {
}

void create_link_text_transformer::do_transform(model::entity& root) {
  model::visitor::visit([&](auto& link, auto&& recurse) {
    using T = std::decay_t<decltype(link)>;

    if constexpr (std::is_same_v<T, model::markup::link>) {
      if (link.children.begin() == link.children.end()) {
        link.target.accept([&](auto&& target) {
          using T = std::decay_t<decltype(target)>;

          const auto apply = [&](const std::string& format, auto&& data) {
            formatter::inja_formatter inja({}, context);

            auto rendered = inja.parse(inja.format(format));

            auto paragraph = rendered.children.begin();

            if (paragraph == rendered.children.end()) {
              // Format string produced an empty markup tree.
              return;
            }

            if (!paragraph->template is<model::markup::paragraph>()) {
              logger::error(fmt::format("Expected anchor to render as a single paragraph but {} rendered as {} instead.", output_generator::xml::xml_generator::render(rendered), output_generator::xml::xml_generator::render(*paragraph)));
              return;
            }

            for (auto& child : paragraph->template as<model::markup::paragraph>().children) {
              link.children.push_back(std::move(child));
            }
          };

          if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
            apply(options.cppast_format, link.target);
          } else if constexpr (std::is_same_v<T, model::link_target::module_target>) {
            apply(options.module_format, link.target);
          } else if constexpr (std::is_same_v<T, model::link_target::sphinx_target>) {
            apply(options.sphinx_format, link.target);
          } else if constexpr (std::is_same_v<T, model::link_target::doxygen_target>) {
            apply(options.doxygen_format, link.target);
          } else if constexpr (std::is_same_v<T, model::link_target::standardese_target>) {
            apply(options.standardese_format, link.target);
          } else if constexpr (std::is_same_v<T, model::link_target::uri_target>) {
            apply(options.uri_format, link.target);
          } else {
            static_assert(always_false_v<T>, "unsupported link target type");
          }
        });
      }
    }

    recurse();
  }, root);
}

}
