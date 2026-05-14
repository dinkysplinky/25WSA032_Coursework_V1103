SHOW = 0

from robots.ecosystem.factory import ecofactory

#defining KPI functions
#collects per-bot stats and returns them as a single fleet-wide dict.

def collect_kpis(es, label="Run"):
    bots = es.bots()

    total_units    = sum(b.units_delivered  for b in bots)
    total_weight   = sum(b.weight_delivered for b in bots)
    total_distance = sum(b.distance         for b in bots)
    total_energy   = sum(b.energy           for b in bots)
    broken_count   = sum(1 for b in bots if b.status == 'broken')
    active_hours   = sum(b.active           for b in bots)

    # max(..., 0.001) stops a divide-by-zero on very short runs.
    kg_per_unit  = total_weight / max(total_distance, 0.001)
    units_per_hr = total_units  / max(active_hours,   0.001)

    return {
        'label':           label,
        'units_delivered': total_units,
        'weight_kg':       round(total_weight,   1),
        'distance':        round(total_distance, 1),
        'energy_kWh':      round(total_energy,   1),
        'broken_bots':     broken_count,
        'active_hours':    active_hours,
        'kg_per_unit':     round(kg_per_unit,  4),
        'units_per_hr':    round(units_per_hr, 4),
    }

# Prints one or more KPI dicts. Extra dicts get a % change column vs the first.
def print_kpi_table(results, title="Bot KPI Summary"):
    kpis = [
        ("Units delivered", "units_delivered", True),
        ("Weight (kg)",     "weight_kg",       True),
        ("Distance",        "distance",        None),
        ("Energy (kWh)",    "energy_kWh",      False),
        ("Broken bots",     "broken_bots",     False),
        ("Active hrs",      "active_hours",    None),
        ("kg / dist",       "kg_per_unit",     True),
        ("Units / hr",      "units_per_hr",    True),
    ]
    label_w, val_w, delta_w = 18, 10, 9
    total_w = label_w + val_w + delta_w

    print(f"\n{'=' * total_w}")
    print(f"{title:^{total_w}}")
    print("=" * total_w)

    baseline = results[0] if results else None
    for i, r in enumerate(results):
        tag = r['label'] if i == 0 else f"{r['label']} vs {baseline['label']}"
        print(f"\n-- {tag} --")
        for name, key, _better in kpis:
            v = r.get(key, '-')
            v_str = f"{v:.2f}" if isinstance(v, float) else f"{v}"
            line  = f"{name:<{label_w}}{v_str:>{val_w}}"
            if i > 0:
                base = baseline.get(key, 0) or 0.0001
                pct  = (r.get(key, 0) - base) / abs(base) * 100
                sign = "+" if pct >= 0 else ""
                line += f"{f'{sign}{pct:.1f}%':>{delta_w}}"
            print(line)
    print("=" * total_w)


#layout for the per-bot table, each tuple is: label, attribute
_BOT_FIELD_GROUPS = [
    ("Identity",  [("name",         "name"),
                   ("kind",         "kind"),
                   ("class",        "kind_class"),
                   ("status",       "status"),
                   ("activity",     "activity"),
                   ("age",          "age")]),
    ("Position",  [("coords",       "coordinates"),
                   ("on_arena",     "on_arena"),
                   ("destination",  "destination"),
                   ("target_dist",  "target_distance"),
                   ("direction",    "direction"),
                   ("distance",     "distance"),
                   ("speed",        "speed"),
                   ("max_speed",    "max_speed"),
                   ("volitant",     "volitant")]),
    ("Energy",    [("soc",          "soc"),
                   ("max_soc",      "max_soc"),
                   ("energy_kWh",   "energy"),
                   ("station",      "station")]),
    ("Delivery",  [("units",        "units_delivered"),
                   ("weight_kg",    "weight_delivered"),
                   ("active_hrs",   "active"),
                   ("damage",       "damage"),
                   ("service_freq", "service_freq"),
                   ("serviced",     "serviced")]),
    ("Capacity",  [("max_payload",  "max_payload"),
                   ("payload",      "payload"),
                   ("weight",       "weight"),
                   ("cargo",        "cargo"),
                   ("contracts",    "contracts"),
                   ("resources",    "resources")]),
]

#formats one value for the per-bot table, long values get cut short
def _fmt_value(v, max_w):
    if isinstance(v, float):
        return f"{v:.2f}"
    if isinstance(v, list):
        flat = all(isinstance(x, (int, float, str, bool)) for x in v)
        if not flat:
            return f"<{len(v)} items>"
        s = str(v)
        return s if len(s) <= max_w else f"<{len(v)} items>"
    s = str(v)
    return s if len(s) <= max_w else s[: max_w - 3] + "..."

#prints a vertical block per bot, easier to read than the wide es.tabulate
def print_per_bot_table(es):
    label_w, val_w = 14, 20
    total_w = label_w + val_w + 2          # +2 for the "  "

    print(f"\n{'=' * total_w}")
    print(f"{'Per-Bot Detail':^{total_w}}")
    print("=" * total_w)

    for bot in sorted(es.bots(),
                      key=lambda b: (b.kind, getattr(b, 'name', ''))):
        name = getattr(bot, 'name', '?')
        print(f"\n{f' {name} ':-^{total_w}}")
        for section, fields in _BOT_FIELD_GROUPS:
            print(f"  [{section}]")
            for label, attr in fields:
                raw = getattr(bot, attr, "-")
                v_str = _fmt_value(raw, val_w)
                print(f"  {label:<{label_w}}{v_str:>{val_w}}")
    print(f"\n{'=' * total_w}")

#euclidean distance so that my calculations work between the 2D and 3D coordinate systems of the drones and other bots
def _euclidean(a, b):
    n = min(len(a), len(b))
    return sum((a[i] - b[i]) ** 2 for i in range(n)) ** 0.5

#returns the charger closest to the bot (None if there aren't any)
def nearest_charger(bot, chargers):
    chargers = list(chargers)
    if not chargers:
        return None
    return min(
        chargers,
        key=lambda c: _euclidean(bot.coordinates, c.coordinates),
    )

OPPORTUNISTIC_CHARGE_RADIUS = 10.0                                              
OPPORTUNISTIC_CHARGE_SOC    = 0.3                                            
 
 #sends the bot to a nearby charger if it's running low on charge and one is close
def try_opportunistic_charge(bot, chargers):                                   
                                                                  
    if bot.station is not None:                                     # already charging           
        return False                                                           
    if not bot.max_soc:                                                        
        return False                                                           
    if bot.soc / bot.max_soc >= OPPORTUNISTIC_CHARGE_SOC:           # SoC is fine
        return False                                                           
                                                                               
    target = nearest_charger(bot, chargers)                                    
    if target is None:                                                         
        return False                                                           
    if _euclidean(bot.coordinates, target.coordinates) > \
            OPPORTUNISTIC_CHARGE_RADIUS:                            # no charger close enough
        return False                                                           
                                                                               
    bot.charge(target)                                                         
    return True                                                                




# Payload utilisation tuning. Stop loading once the bot is at or above
# this fraction of its max_payload - leaves a small safety margin so a
# slightly heavier pizza doesn't push the bot over capacity.
PAYLOAD_FILL_TARGET = 0.95                                                     

#loads as many ready pizzas onto a bot until max_payload is reached
def load_bot_to_capacity(bot, deliverables):                                   
    capacity = getattr(bot, 'max_payload', 0)                                  
    if capacity <= 0:                                                          
        return 0.0                                                           
                                                                               
    planned_load = 0.0                                                         
    fill_ceiling = PAYLOAD_FILL_TARGET * capacity                              
                                                                               
    for pizza in deliverables:                                                 
        if pizza.status != 'ready':                                            
            continue                                                           
        p_weight = getattr(pizza, 'weight', 0) or 0                            
                                                                               
        # Skip pizzas that would push us over max_payload. keep iterating in   
        # case a lighter one further down the list still fits.                 
        if planned_load + p_weight > capacity:                                 
            continue                                                           
                                                                               
        bot.deliver(pizza)                                                     
        planned_load += p_weight                                               
                                                                               
        # Stop once close enough to full to avoid spending cycles        
        # scanning for tiny leftovers.                                         
        if planned_load >= fill_ceiling:                                       
            break                                                              
    return planned_load                                                        


import matplotlib.pyplot as plt
plt.close('all')        # optional: cleans up leftovers from prior runs
plt.ion()               # interactive mode ON (non-blocking windows)

# Create and configure the ecosystem using the factory function.
# Study the factory function code to understand how the ecosystem is being
# created and configured. Adjust the parameters as needed for testing.
es = ecofactory(robots=3, droids=3, drones=3, chargers=[[40, 20], [10, 10], [70, 30], [25, 40]], pizzas=9)

charger = es.chargers()[0]

# show = 0  -> no display, fastest run; set 1 for development/debugging.
# When show = 0 it is wise to turn messages on for shorter dev runs.
es.display(show=0, pause=10)
es.debug       = False                  # damage / warning messages (needs show=0)
es.messages_on = False                  # for 52 weeks; turn on for short runs
es.duration    = "52 week"              # aim for a year with few breakages

# Home is the point bots return to when idle; also the charger location here.
home = [40, 20, 0]

# SoC percentage at which bots decide to charge. Can be optimised per kind
# (stretch objective).
charge_threshold = 0.1

while es.active:

    for bot in es.bots():

        # create_deliverables(es)   # maintain a stock of ready pizzas

        # Decide to charge when soc < threshold and not already at a station.
        if bot.soc / bot.max_soc < charge_threshold and bot.station is None:
            target = nearest_charger(bot, es.chargers())
            if target is not None:
                bot.charge(target)
        else:                                                                   
            try_opportunistic_charge(bot, es.chargers())   

        # If idle, contract to deliver a ready pizza.
        if bot.activity == 'idle':
            load_bot_to_capacity(bot, es.deliverables())                        
            # If no pizza was loaded, head home as before.
            if not bot.destination and bot.coordinates != home:
                bot.target_destination = home

        # Move while we have a destination. At end of delivery the bot
        # activity will be set back to idle.
        if bot.target_destination:
            bot.move()

    es.update()                # update once all bots have been processed

kpis = collect_kpis(es, label="Baseline")
print_kpi_table([kpis], title="Bot Delivery KPI Summary")

# Per-bot detailed breakdown
#print_per_bot_table(es)