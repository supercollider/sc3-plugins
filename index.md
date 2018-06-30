# Releases
{% assign latest=site.data.releases.first %}
Download latest release: <a class="btn" href="{{ site.release_baseurl }}/download/{{ latest.tag }}/{{ latest.link }}">{{ latest.tag }}</a>

Older releases can be found on [github releases page]({{ site.release_baseurl }}).

{% include_relative README.md %}
