serve: md bundle
	bundle exec jekyll serve -d generated/html

bundle:
	bundle config set path vendor/bundle
	bundle install

html: md bundle
	bundle exec jekyll build -d generated/html

md:
	standardese ../src --output.prefix generated/markdown/

.PHONY: md html serve bundle
