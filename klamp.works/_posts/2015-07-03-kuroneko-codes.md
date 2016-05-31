---
layout: post
title:  "Kuroneko's Code"
date: 2015-07-03 20:07:56
categories: 
---
In the Oreimo OVAs (Episode 14), Kuroneko creates her own Visual Novel and in one scene we get a tantalising glimpse of the source code. Just one question though.

![cap_Ore_no_Imouto_ga_Konna_ni_Kawaii_Wake_ga_Nai-Ep14_00.24.54_01.jpg]({{ site.baseurl }}/images/cap_Ore_no_Imouto_ga_Konna_ni_Kawaii_Wake_ga_Nai-Ep14_00.24.54_01.jpg)

    bc=(ay*ax*az)
    check1 = ((ay+acy)*(ax+acx)*(az+acz))
    check2 = p_ar.count-9
    if check1!=check2 then return(messagebox "rebuild")
    
    
    --position
    dmorr1=#()
    dmar2=#()
    dmar3=#()
    
    --Y update
    if acy>0 and bc
    (
        dmar1=#()
        dmar2=#()
        rnum = (bp+ch)
        for i=1 to pcount do
        (
            if (sag m)!=0 then
            (
                append dmar2(getUserProp cnode("cp"+(i as string)))
            )
            else
            (
                append dmar2(getUserProp dconde("cp"+(i as string)))
                append imgt1 dmar2
                dmar2=#()
            )


What flipping programming language is that? 

Fortunately when the series aired I was a student and had a lot of free time on my hands. Trying to answer this question was a surprisingly remarkable experience and exposed me to many programming languages I may not have otherwise come into contact with. 

There are no semi colons at the end of lines It uses the keywords "if", "then", "for", "to", "do" but not "end" It uses the keyword "as" so is probably Object Oriented. So far it might be F# or Haskell syntax however it also uses #0† which looks like Pascal-style notation for representing chars, i.e. #0 would be the equivalent of '\0' in C. 

In the end I couldn't pin it down all of the weird attributes to a single language so I followed the age old tradition of asking [StackOverflow](https://stackoverflow.com/questions/15841181/mysterious-programming-language). I feel compelled to explain that I am not some kind of pathological liar. My original question, explaining the true source of the code was met with the typical aggression, animosity, rage and general misery one grows to expect every second of every day on StackOverflow. Interesting though that the exact same question with slightly different wording generated a much more positive response; an early lesson for me in how shallow human beings are and the value of appearances.

The consensus was that the language is called [MAXScript](https://en.wikipedia.org/wiki/MAXScript) - a somewhat obscure propitiatory scripting language for a 3D computer graphics product.

A disappointing result to be honest. Maybe some dedicated fan will create an actual useful general purpose programming language with similar syntax just for the fandomity of it. Maybe.

---

† Actually it's #() which means empty array but that's quite hard to see even in the 1080p screenshot.
