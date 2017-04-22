echo '|#|Anime Title|Score|Type|Progress|' > ../_includes/anime.tab
echo '|-|-----------|-----|----|--------|' >> ../_includes/anime.tab
sort ../anime.raw | awk '{printf("|%03d|%s\n", NR, $0)}' >> ../_includes/anime.tab


