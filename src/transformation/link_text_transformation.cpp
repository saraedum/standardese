// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "../../standardese/transformation/link_text_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/link_target.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

template<class> inline constexpr bool always_false_v = false;

namespace standardese::transformation {

void link_text_transformation::do_transform(model::entity& root) {
  model::visitor::visit([&](auto& link, auto&& recurse) {
    using T = std::decay_t<decltype(link)>;

    if constexpr (std::is_same_v<T, model::markup::link>) {
      if (link.begin() == link.end()) {
        link.target.accept([&](auto&& target) {
          using T = std::decay_t<decltype(target)>;

          const auto apply = [&](const std::string& format, auto&& data) {
            formatter::inja_formatter inja({});
            inja.data().merge_patch(inja.to_json(data));

            auto rendered = inja.build(format);

            auto paragraph = rendered.begin();

            if (paragraph == rendered.end()) {
              // Format string produced an empty markup tree.
              return;
            }

            if (!paragraph->template is<model::markup::paragraph>()) {
              logger::error(fmt::format("Expected anchor to render as a single paragraph but {} rendered as {} instead.", output_generator::xml::xml_generator::render(rendered), output_generator::xml::xml_generator::render(*paragraph)));
              return;
            }

            for (auto& child : paragraph->template as<model::markup::paragraph>()) {
              link.add_child(std::move(child));
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
