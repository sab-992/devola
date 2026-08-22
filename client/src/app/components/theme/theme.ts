import { Component, effect, inject } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { ThemeService } from '@services/theme/theme';

@Component({
    selector: 'app-theme',
    imports: [MatIconModule],
    templateUrl: './theme.html',
    styleUrl: './theme.scss',
})
export class ThemeComponent {
    private m_themeService: ThemeService = inject(ThemeService);

    constructor() {
        effect(this.m_themeService.saveTheme.bind(this.m_themeService));
    }

    get theme() {
        return this.m_themeService.theme;
    }

    toggleTheme(): void {
        this.m_themeService.toggle();
    }
}
