# Understanding the vehicles code

## Major structures

### vehicle class
Each vehicle is stored in an instance of the vehicle class.  The vehicle class contains tilerays for facing and movement, integers for current speed and cruise speed setpoint in hundredths of miles per hour, and a vector of vehicle_parts.

### vehicle_part class
Every component of a vehicle is stored in an instance of the vehicle_part class.  Each one has a reference to its vpart_info, a point mount that has the x, y coordinates of the component in the vehicle's internal grid, a 2 member array of points named precalc which has the pretranslated values of mount to the vehicle's face.dir and turn.dir, and a item instance named base which is the item used to install the part.

### vpart_info class
vpart_info stores the JSON data for each type of vehicle component.

## Code flow
Each turn, vehicles are updated in the following sequence:

game::do_turn()
  map::process_falling()
    map::drop_everything()
      map::drop_vehicle()
        if unsupported vehicle at location
          vehicle.falling = true 
  map::veh_move()
    for vehicles in the vehicles list
      vehicle::gain_moves()
        vehicle::shed_loose_parts()
        vehicle::thrust()
          vehicle::slowdown()
            vehicle::coeff_air_drag()
            vehicle::coeff_water_drag()
            vehicle::coeff_rolling_drag()
          calculate desired_accel from vehicle::current_acceleration() and vehicle::k_traction( map::vehicle_wheel_traction ), capped by cruise_velocity and max_velocity
          calculate load as desired_accel / accel
          vehicle::noise_and_smoke()
          vehicle::consume_fuel()
          vehicle::do_damage_engine() from vehicle::strain()
        vehicle::traverse_vehicle_graph()
        vehicle::do_environmental_effects()
        vehicle::automatic_fire_turret()
      vehicle::slow_leak()
    for 0 to 100 or until no vehicles can proceed
      map::vehproceed()
        for vehicles in the vehicle list
          vehicle::act_on_map()
             map::vehicle_wheel_traction()
             map::move_vehicle()
               vehicle::precalc_mounts()
               ** collison code **
               vehicle::handle_trap()
               vehicle::on_move()
                 vehicle::operate_scoop/planter/plow/reaper/rockwheel()
               map::displace_vehicle()
                 vehicle::shed_loose_parts
                 map::update_vehicle_cache()
                 map::on_vehicle_moved()
    for vehicles in the dirty vehicle list
      vehicle::part_removal_cleanup()
  for submaps in the MAPBUFFER
    for vehicles in the reality bubble
      vehicle::power_parts()
        sum power of parts that consume epower
        sum power of parts that produce power
        consume or add battery energy as appropriate
      vehicle::idle()
        vehicle::consume_fuel() from alternator load
        vehicle::noise_and_smoke()
        vehicle::play_music()
        vehicle::play_chimes()
        vehicle::alarm()
        vehicle::update_time()
          vehicle::discharge_battery() from water purify
          vehicle::charge_battery() from solar panels
  player::process_active_items()

