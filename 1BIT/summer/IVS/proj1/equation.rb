class Equation
    def self.solve_quadratic(a, b, c)
        if a == 0 then
        	if b == 0 then
        		if c == 0 then
        			return [Infinity] # 0x^2 + 0x + 0 = 0
        		else
        			return nil # c = 0
        		end
        	else
        		return [(-c/b.to_f)] # bx + c = 0
        	end
        else
        	d = ((b*b)-(4*a*c)) # ax^2 + bx + c = 0
        	if d == 0 then # 1 koren
        		return [(-b/2*a.to_f)]
        	elsif d < 0 then # v obore R nema riesenie
        		return nil
        	else # 2 korene
        		return [(-b+Math.sqrt(d))/(2*a.to_f),(-b-Math.sqrt(d))/(2*a.to_f)]
        	end
        end
    end
end