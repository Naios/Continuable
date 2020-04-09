(function() {
  var backend = '// Continuable for backend developers:\n(http_request("github.com/Naios") && http_request("atom.io"))\n  .then([] (std::string github, std::string atom) {\n    // ...\n    return mysql_query("SELECT * FROM `users`");\n  })\n  .then([] (ResultSet result) {\n    // ...\n  }, executor->post());';
  var game = '// Continuable for game developers:\nme->WaitForPlayer()\n  .then([&] (Player* player) {\n    return me->MoveTo(player->GetPosition());\n  })\n  .then(me->Say("Hello friend!"))\n  .then(me->Wait(3s)))\n  .then(me->MoveTo(me->GetHomePosition()));';
  var adventurous = '// Continuable for the adventurous:\ntry {\n  auto response = co_await http_request("github.com/Naios");\n} catch (std::exception const& e) {\n  // ...\n}\nauto c = cti::make_ready_continuable(0, 1);\nauto [ first, second ] = co_await std::move(c);\nco_return first + second;';
  var coroutines = '// Continuable for mastering coroutines:\ncti::when_all(\n    []() -> cti::continuable<int, int> {\n      co_return std::make_tuple(0, 1); // ...\n    }(), []() -> cti::continuable<int, int> {\n      co_return std::make_tuple(2, 3); // ...\n    }()).then([](int, int, int, int) {\n      // ...\n    });';

  // Teach hljs some new keywords...
  var cpp = hljs.getLanguage('cpp').exports.k;
  cpp.built_in +=
    " http_request mysql_query then exception post make_continuable cti make_ready_continuable ResultSet move WaitForPlayer MoveTo Player GetPosition GetHomePosition Wait Say when_all continuable make_tuple";
  cpp.keyword +=
    " co_await co_return";
  cpp.literal +=
    " 3s";

  hljs.configure({
    tabReplace: '  ',
    // classPrefix: '',
    languages: ['cpp'],
  });

  function highlight(code) {
    return hljs.highlight('cpp', code, "", null).value;
  };

  var typed = new Typed('#typedterminal', {
    strings: [
      highlight(backend),
      highlight(game),
      highlight(adventurous),
      highlight(coroutines),
    ],
    typeSpeed: 6,
    backSpeed: 4,
    startDelay: 1100,
    loop: true,
    shuffle: true,
    backDelay: 5500,
    // smartBackspace: true,
    // fadeOut: true,
    // fadeOutClass: 'typed-fade-out',
    // fadeOutDelay: 5500,
  });
})();
