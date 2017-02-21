require 'rubygems'
require 'nokogiri'

html = open("index.html").read

doc = Nokogiri::HTML(html, nil, 'utf-8') do |config|
    config.noent.nonet
end

tables = doc.css('body/div/table')[3..-1]
tables = tables.reverse[1..-1].reverse

puts "|#|Anime Title|Score|Type|Progress|"
puts "|-|-----------|-----|----|--------|"

tables.each { |table|
    tds = table.css('td')
    num = tds[0].text.strip
    url = tds[1].css("a")[2]['href'].strip
    title = tds[1].css("a")[2].text.strip
    score = tds[2].text.strip
    type = tds[3].text.strip
    eps = tds[4].text.strip

    puts "|#{num}|[#{title}](http://myanimelist.net#{url})|#{score}|#{type}|#{eps}|"
}


