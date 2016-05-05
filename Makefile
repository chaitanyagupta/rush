dir_guard = @mkdir -p $(@D)

bin_names = rush dumb execve1 fork1 printargs exec-fool printenv search_path forkme execme execprintargs
bins := $(patsubst %,bin/%,$(bin_names))

all: $(bins) presentation

.PHONY: clean
clean:
	rm -rf bin/

$(bins): bin/%: src/%.c
	$(dir_guard)
	cc -o $@ $^

bin/vfork1: bin/fork1
	$(dir_guard)
	ln -sf $$(basename $^) $@

.PHONY: presentation
presentation: presentation/index.html presentation/index.css

presentation/index.html: presentation.md
	$(dir_guard)
	remark-generate-slides $^ $@ index.css

presentation/index.css: presentation.css
	$(dir_guard)
	cp $^ $@
