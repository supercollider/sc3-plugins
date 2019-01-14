# Releases
{% assign latest=site.data.releases.first %}
Download latest release: <a class="btn" href="{{ site.release_baseurl }}/tag/{{ latest.tag }}">{{ latest.tag }}</a>

Older releases can be found on [github releases page]({{ site.release_baseurl }}).

{% include_relative README.md %}

# About this website

To locally build and modify this [`jekyll`](https://jekyllrb.com) site you'll need the following ruby gems:

```bash
$ gem install bundler jekyll
```

Then, inside the repository main folder:

```bash
$ bundle install
```

and follow the instructions.

### Adding a new release

Add the release **at the top** of the releases page (`_data/releases.yml`), so that the main download
button link will be automagically updated :)
