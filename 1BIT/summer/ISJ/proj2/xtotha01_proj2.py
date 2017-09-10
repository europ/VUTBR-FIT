#!/usr/bin/env python3

############################
# ISJ - Skriptovaci jazyky #
# Projekt 2                #
# xtotha01 - Adrian Toth   #
# 11.4.2016                #
############################

class Polynomial:

    # List
    polynom = []

    # Inicializuje polynom
    def __init__(self, *args, **kwargs):
        self.polynom = []

        # input is tuple of list
        if (len(args) == 1 and type(args[0]) is list):
            self.polynom = list(args[0])

        # input is tuple
        elif (len(args) >= 1 and type(args[0]) is int):
            for i in range(len(args)):
                self.polynom.append(args[i])

        # input is dictionary
        else:
            for key in kwargs:
                if (int(key[1:]) < len(self.polynom)):
                    self.polynom[int(key[1:])] = kwargs[key]
                else:
                    d = len(self.polynom)
                    for j in range( int(key[1:]) +1 - d ):
                        if j == ( int(key[1:]) - d):
                            self.polynom.append(kwargs[key])
                        else:
                            self.polynom.append(0)

    # Vypise polynom
    # ==================================================================
    # idx = mocnina neznamej
    # hodnot = coefficient
    # - ak idx==(len(list)-1) <=> list[..., idx] tak
    #   - ak hodnota(list[..., idx]) > 0 tak vypise 'value_x^_idx'
    #   - ak hodnota(list[..., idx]) < 0 tak vypise '- value_x^_idx'
    # - ak idx==0 <=> list[idx, ...] tak
    #   - ak hodnota(list[idx, ...]) == 0 tak nic nevypise
    #   - ak hodnota(list[idx, ...]) > 0 tak vypise ' + value'
    #   - ak hodnota(list[idx, ...]) < 0 tak vypise ' - value'
    # - ak idx==1 <=> list[#, idx, ...] tak
    #   - ak hodnota(list[idx, ...]) == 0 tak nic nevypise
    #   - ak hodnota(list[idx, ...]) > 0 tak vypise ' + value_x'
    #   - ak hodnota(list[idx, ...]) < 0 tak vypise ' - value_x'
    # - ak idx>1 and idx<(len(list)-1) <=> list[..., idx, ...] tak
    #   - ak hodnota list[idx]==1 tak vypise ' + x^idx'
    #   - ak hodnota list[idx]==-1 tak vypise ' - x^idx'
    #   - ak hodnota list[idx]==0 tak nic nevypise
    def __str__(self):
        result = []
        for i, value in reversed(list(enumerate(self.polynom))):
            if value == 0:
                continue
            elif value < 0:
                result.append('-')
            else:
                if result:
                    # Na zaciatku to nechcem
                    result.append('+')
            value = abs(value)
            if value == 1 and i != 0:
                # Nevypisujem
                value = ''
            if i > 1:
                result.append('{}x^{}'.format(value, i))
            elif i == 1:
                result.append('{}x'.format(value))
            else:  # 0
                result.append('{}'.format(value))
        return ' '.join(result)

    # Scita (polynom1 + polynom2)
    # - vytvori vysledny polynom potrebnej dlzky
    # - scita rovnake indexy polynom1 a polynom2
    # - vysledok ulozi na index pomocou ktoreho scitoval
    def __add__(self, other):
        pol_sum = []
        pol1 = self.polynom
        pol2 = other.polynom

        # nastavy pozadovanu velkost pol_sum
        if len(pol1) >= len(pol2):
            for i in range(len(pol1)):
                pol_sum.append(0)
        else:
            for i in range(len(pol2)):
                pol_sum.append(0)

        for i in range(len(pol1)):
            pol_sum[i] = pol_sum[i] + pol1[i]
        for i in range(len(pol2)):
            pol_sum[i] = pol_sum[i] + pol2[i]

        return Polynomial(pol_sum)

    # Vynasobi (polynom * polynom)
    # - vynasobi kazdy clen polynom1 s kazdym clenom polynom2
    # - hodnotu pricita na index zodpovedajuci hodnote
    def __mul__(self, other):
        new_polynom = []
        pol1 = other.polynom
        pol2 = self.polynom

        # nastavy pozadovanu velkost new_polynom
        # (podla najvyssej mocniny ktora moze nastat)
        for k in range(len(pol1)+len(pol2)):
                new_polynom.append(0)

        # nasobenie pol1 a pol2
        # nasobenie kazdeho clena s kazdym
        for i in range(len(pol1)):
            for j in range(len(pol2)):
                old_value = new_polynom[i+j]
                new_value = old_value + (pol1[i] * pol2[j])
                new_polynom[i+j] = new_value

        return Polynomial(new_polynom)

    # Umocni polynom (polynom ** exponent)
    # - vynasobi (polynom*polynom*...) exponent-krat
    def __pow__(self, exponent):
        # Check params
        if type(exponent) is not int:
            return "ERROR"

        # Check params was successful
        else:
            if exponent < 0:
                return "ERROR"
            elif exponent == 0:
                return 1
            elif exponent == 1:
                return self
            else:
                output = self * self
                for i in range(exponent-2):
                    output = output * self
                return output

    # Zderivuje polynom ((polynom)dx)
    # - vynasobi polynom indexom
    # - posunie polynom o 1 do lava [<==], (zmensi listo o 1)
    def derivative(self):
        pol_derivative = []
        for idx in range(len(self.polynom)):
            pol_derivative.append(self.polynom[idx]*idx)
        return Polynomial(pol_derivative[1:])

    # Vypocita hodnotu v bode (polynom(number))
    # - vynasobi index s hodnoutou ulozenou na indexe
    # - scita vysledky sucinu
    def at_value(self, *args):
        # Check params
        for i in range(len(args)):
            if not (type(args[i]) == int or type(args[i]) == float):
                return "ERROR"

        # Check params was successful
        else:
            # polynom(x)
            if len(args) == 1:
                # polynom(x)
                number = args[0]
                value = 0
                for idx in range(len(self.polynom)):
                    coefficient = self.polynom[idx]
                    value = value + ( coefficient * (number ** idx) )

                # return polynom(x)
                return value

            # polynom(y) - polynom(x)
            elif len(args) == 2:
                # polynom(y)
                number1 = args[0]
                value1 = 0
                for idx in range(len(self.polynom)):
                    coefficient = self.polynom[idx]
                    value1 = value1 + ( coefficient * (number1 ** idx) )

                # polynom(x)
                number2 = args[1]
                value2 = 0
                for idx in range(len(self.polynom)):
                    coefficient = self.polynom[idx]
                    value2 = value2 + ( coefficient * (number2 ** idx) )

                # return ( polynom(y) - polynom(x) )
                value = value2 - value1
                return value

            # Fault
            else:
                return "ERROR"
